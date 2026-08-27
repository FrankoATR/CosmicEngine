#include "chase_camera.hpp"

#include "../entities/ship.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include CAMERAMANAGER_HEADER

#include <algorithm>

namespace
{
    constexpr float kLagDistance = 6.0f;
    constexpr float kCameraBack = 46.0f;
    constexpr float kCameraAbove = 14.0f;
    constexpr float kLookAhead = 40.0f;
    constexpr float kLateralFollow = 0.5f;
    constexpr float kSmoothRate = 8.0f;
}

namespace NaBazu
{
    void ChaseCamera::Reset()
    {
        initialized_ = false;
    }

    void ChaseCamera::Update(const Ship &ship, const RailPath &rail, float deltaTime)
    {
        const float camDistance = std::max(0.0f, ship.GetDistanceTraveled() - kLagDistance);
        const RailFrame frame = rail.GetFrameAtDistance(camDistance);

        const glm::vec3 ideal = frame.position + frame.up * kCameraAbove - frame.forward * kCameraBack +
                                 frame.right * (ship.GetLateralOffset() * kLateralFollow);

        const glm::vec3 lookTarget = ship.GetPosition() + ship.GetCurrentRailFrame().forward * kLookAhead;

        if (initialized_)
        {
            const float alpha = glm::clamp(kSmoothRate * deltaTime, 0.0f, 1.0f);
            smoothedPosition_ = glm::mix(smoothedPosition_, ideal, alpha);
            // The look-at target was previously applied unsmoothed while the camera's
            // position lagged behind via smoothedPosition_ -- that mismatch (a lagging
            // eye fixed on a non-lagging target) is what read as a jerky/abrupt camera,
            // especially through curves. Smooth both the same way.
            smoothedLookTarget_ = glm::mix(smoothedLookTarget_, lookTarget, alpha);
        }
        else
        {
            smoothedPosition_ = ideal;
            smoothedLookTarget_ = lookTarget;
            initialized_ = true;
        }

        CAM_MN.SetFocusPosition(smoothedPosition_, smoothedLookTarget_);
    }
}
