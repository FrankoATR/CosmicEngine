#include "rail_path.hpp"

#include <algorithm>
#include <cmath>

namespace NaBazu
{
    RailPath::RailPath(std::vector<glm::vec3> waypoints, int samplesPerSegment, glm::vec3 worldUp)
        : worldUp_(glm::normalize(worldUp)),
          boundsMin_(0.0f),
          boundsMax_(0.0f),
          samplesPerSegment_(std::max(2, samplesPerSegment))
    {
        if (waypoints.size() < 2)
        {
            waypoints.push_back(waypoints.empty() ? glm::vec3(0.0f) : waypoints.front() + glm::vec3(0.0f, 0.0f, -1.0f));
        }

        controlPoints_.reserve(waypoints.size() + 2);
        controlPoints_.push_back(waypoints.front());
        controlPoints_.insert(controlPoints_.end(), waypoints.begin(), waypoints.end());
        controlPoints_.push_back(waypoints.back());

        // At t=0 EvaluateCatmullRom(p0,p1,p2,p3,0) collapses to exactly p1 -- i.e. the
        // first real waypoint -- so this seeds the LUT with the true starting point.
        const glm::vec3 startPoint = EvaluateCatmullRom(controlPoints_[0], controlPoints_[1], controlPoints_[2], controlPoints_[3], 0.0f);
        lut_.push_back({0.0f, startPoint});
        boundsMin_ = startPoint;
        boundsMax_ = startPoint;

        const int numSegments = static_cast<int>(controlPoints_.size()) - 3;
        if (numSegments > 0)
        {
            AppendArcLengthSegments(0, numSegments);
        }
    }

    void RailPath::AppendWaypoints(const std::vector<glm::vec3> &newWaypoints)
    {
        if (newWaypoints.empty() || controlPoints_.size() < 4)
        {
            return;
        }

        const int oldNumSegments = static_cast<int>(controlPoints_.size()) - 3;
        // The last old segment's shape depended on the duplicated tail endpoint we're
        // about to replace with a real point, so it must be recomputed too.
        const int recomputeStartSegment = std::max(0, oldNumSegments - 1);

        controlPoints_.pop_back(); // drop the old duplicate-last
        controlPoints_.insert(controlPoints_.end(), newWaypoints.begin(), newWaypoints.end());
        controlPoints_.push_back(newWaypoints.back()); // new duplicate-last

        const int newNumSegments = static_cast<int>(controlPoints_.size()) - 3;

        const std::size_t keepCount = 1 + static_cast<std::size_t>(recomputeStartSegment) * static_cast<std::size_t>(samplesPerSegment_);
        if (keepCount < lut_.size())
        {
            lut_.resize(keepCount);
        }

        AppendArcLengthSegments(recomputeStartSegment, newNumSegments);
    }

    glm::vec3 RailPath::EvaluateCatmullRom(const glm::vec3 &p0, const glm::vec3 &p1,
                                            const glm::vec3 &p2, const glm::vec3 &p3, float t)
    {
        const float t2 = t * t;
        const float t3 = t2 * t;

        return 0.5f * ((2.0f * p1) +
                       (-p0 + p2) * t +
                       (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 +
                       (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
    }

    void RailPath::AppendArcLengthSegments(int startSegment, int endSegmentExclusive)
    {
        if (lut_.empty())
        {
            return; // constructor always seeds lut_ with the start point first
        }

        glm::vec3 previousPoint = lut_.back().position;
        float accumulated = lut_.back().distance;

        for (int segment = startSegment; segment < endSegmentExclusive; ++segment)
        {
            const glm::vec3 &p0 = controlPoints_[segment];
            const glm::vec3 &p1 = controlPoints_[segment + 1];
            const glm::vec3 &p2 = controlPoints_[segment + 2];
            const glm::vec3 &p3 = controlPoints_[segment + 3];

            for (int sample = 1; sample <= samplesPerSegment_; ++sample)
            {
                const float t = static_cast<float>(sample) / static_cast<float>(samplesPerSegment_);
                const glm::vec3 point = EvaluateCatmullRom(p0, p1, p2, p3, t);

                accumulated += glm::distance(point, previousPoint);
                lut_.push_back({accumulated, point});
                previousPoint = point;

                boundsMin_ = glm::min(boundsMin_, point);
                boundsMax_ = glm::max(boundsMax_, point);
            }
        }
    }

    float RailPath::GetTotalLength() const
    {
        return lut_.empty() ? 0.0f : lut_.back().distance;
    }

    glm::vec3 RailPath::GetPointAtDistance(float distance) const
    {
        if (lut_.empty())
        {
            return glm::vec3(0.0f);
        }

        const float clamped = std::clamp(distance, 0.0f, GetTotalLength());

        auto it = std::lower_bound(lut_.begin(), lut_.end(), clamped,
            [](const ArcSample &sample, float value) { return sample.distance < value; });

        if (it == lut_.begin())
        {
            return lut_.front().position;
        }
        if (it == lut_.end())
        {
            return lut_.back().position;
        }

        const ArcSample &upper = *it;
        const ArcSample &lower = *(it - 1);
        const float span = upper.distance - lower.distance;
        const float alpha = (span > 1e-6f) ? (clamped - lower.distance) / span : 0.0f;
        return glm::mix(lower.position, upper.position, alpha);
    }

    RailFrame RailPath::GetFrameAtDistance(float distance) const
    {
        const float total = GetTotalLength();
        const float d = std::clamp(distance, 0.0f, total);
        // Spans several LUT brackets (not just one) so the finite-difference tangent
        // blends across them instead of tracking a single bracket's constant slope --
        // see the samplesPerSegment comment in rail_path.hpp for why that matters.
        constexpr float kEpsilon = 2.0f;

        const glm::vec3 behind = GetPointAtDistance(std::clamp(d - kEpsilon, 0.0f, total));
        const glm::vec3 ahead = GetPointAtDistance(std::clamp(d + kEpsilon, 0.0f, total));

        RailFrame frame;
        frame.position = GetPointAtDistance(d);

        const glm::vec3 delta = ahead - behind;
        frame.forward = (glm::length(delta) > 1e-5f) ? glm::normalize(delta) : glm::vec3(0.0f, 0.0f, -1.0f);

        glm::vec3 right = glm::cross(frame.forward, worldUp_);
        if (glm::length(right) < 1e-4f)
        {
            // Forward is nearly parallel to worldUp_ (near-vertical stretch of the rail);
            // fall back to a stable reference axis instead of producing a degenerate right vector.
            right = glm::cross(frame.forward, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        frame.right = glm::normalize(right);
        frame.up = glm::normalize(glm::cross(frame.right, frame.forward));

        return frame;
    }

    glm::vec3 RailPath::GetBoundsMin() const
    {
        return boundsMin_;
    }

    glm::vec3 RailPath::GetBoundsMax() const
    {
        return boundsMax_;
    }
}
