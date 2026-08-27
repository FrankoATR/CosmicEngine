#ifndef NABAZU_RAIL_PATH_HPP
#define NABAZU_RAIL_PATH_HPP

#include <glm/glm.hpp>

#include <vector>

namespace NaBazu
{
    struct RailFrame
    {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    };

    // Curved rail built from a Catmull-Rom spline through a list of waypoints,
    // re-parameterized by arc length via a precomputed lookup table (LUT). This lets
    // the ship advance at a constant world-space speed and lets waves be triggered by
    // true distance traveled, rather than by the spline's raw (non-uniform) parameter.
    class RailPath
    {
    public:
        // samplesPerSegment default is deliberately high (not 20): GetFrameAtDistance
        // derives "forward" by finite-differencing GetPointAtDistance, which linearly
        // interpolates *within* a single LUT bracket. With a coarse LUT, that estimate
        // is piecewise-constant across each bracket and snaps at bracket boundaries --
        // visible as small periodic jerks in the ship's derived orientation as it
        // travels. A finer LUT shrinks each snap below the perceptible threshold.
        explicit RailPath(std::vector<glm::vec3> waypoints,
                           int samplesPerSegment = 64,
                           glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));

        float GetTotalLength() const;
        glm::vec3 GetPointAtDistance(float distance) const;
        RailFrame GetFrameAtDistance(float distance) const;
        glm::vec3 GetBoundsMin() const;
        glm::vec3 GetBoundsMax() const;

        // Fase 2 (nivel infinito): extends the curve with more real waypoints appended
        // after the current tail. The LAST previously-built segment is recomputed (its
        // shape depended on a placeholder duplicated endpoint that is now replaced by a
        // real continuation), everything before that is left untouched, so distances
        // already traveled by the ship stay valid.
        void AppendWaypoints(const std::vector<glm::vec3> &newWaypoints);

    private:
        struct ArcSample
        {
            float distance;
            glm::vec3 position;
        };

        static glm::vec3 EvaluateCatmullRom(const glm::vec3 &p0, const glm::vec3 &p1,
                                             const glm::vec3 &p2, const glm::vec3 &p3, float t);
        // Appends LUT samples for segments [startSegment, endSegmentExclusive), continuing
        // from lut_.back() (or from t=0 of startSegment if lut_ is empty).
        void AppendArcLengthSegments(int startSegment, int endSegmentExclusive);

        std::vector<glm::vec3> controlPoints_;
        std::vector<ArcSample> lut_;
        glm::vec3 worldUp_;
        glm::vec3 boundsMin_;
        glm::vec3 boundsMax_;
        int samplesPerSegment_;
    };
}

#endif
