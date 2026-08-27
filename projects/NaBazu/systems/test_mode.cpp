#include "test_mode.hpp"

#include "../entities/ship.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include CAMERAMANAGER_HEADER
#include GAMEMANAGE_HEADER
#include INPUTMANAGER_HEADER
#include OBJECTMANAGER_HEADER

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace
{
    bool s_testModeEnabled = false; // normal play by default; F3 opts in
    bool s_worldFrozen = false;

    // Velocities parked while frozen, keyed by object id, so unfreezing restores the
    // exact motion instead of leaving everything dead in the air.
    std::unordered_map<int, glm::vec3> s_parkedVelocities;

    constexpr float kOrbitSensitivity = 0.25f;
    constexpr float kMinPitchDeg = -85.0f;
    constexpr float kMaxPitchDeg = 85.0f;
    constexpr float kMinDistance = 15.0f;
    constexpr float kMaxDistance = 400.0f;
}

namespace NaBazu
{
    bool IsTestModeEnabled()
    {
        return s_testModeEnabled;
    }

    void SetTestModeEnabled(bool enabled)
    {
        s_testModeEnabled = enabled;
    }

    bool IsWorldFrozen()
    {
        return s_worldFrozen;
    }

    void ToggleWorldFrozen()
    {
        s_worldFrozen = !s_worldFrozen;

        if (s_worldFrozen)
        {
            s_parkedVelocities.clear();
            for (auto *object : OBJ_MN.GetAll())
            {
                // The player keeps moving -- the whole point is to fly around and
                // inspect frozen traffic.
                if (!object || object->GetClassName() == Ship::StaticClassName())
                {
                    continue;
                }

                const glm::vec3 velocity = object->GetVelocity();
                if (glm::length(velocity) > 0.0f)
                {
                    s_parkedVelocities[object->GetID()] = velocity;
                    object->SetVelocity(glm::vec3(0.0f));
                }
            }
            return;
        }

        for (auto *object : OBJ_MN.GetAll())
        {
            if (!object)
            {
                continue;
            }

            auto it = s_parkedVelocities.find(object->GetID());
            if (it != s_parkedVelocities.end())
            {
                object->SetVelocity(it->second);
            }
        }
        s_parkedVelocities.clear();
    }

    void OrbitCamera::Attach()
    {
        if (attached_)
        {
            return;
        }
        attached_ = true;

        INP_MN.SetActiveMouseInput();
        INP_MN.SetDisableMouse(true);   // capture the cursor for free-look
        INP_MN.ResetMouseLookReference(); // avoid a huge first delta

        GM_MN.setMousePositionCallback([this](double xpos, double ypos)
        {
            float xoffset = 0.0f;
            float yoffset = 0.0f;
            if (!INP_MN.TryGetMouseLookDelta(static_cast<float>(xpos), static_cast<float>(ypos), xoffset, yoffset))
            {
                return;
            }

            yawDeg_ += xoffset * kOrbitSensitivity;
            pitchDeg_ = std::clamp(pitchDeg_ + yoffset * kOrbitSensitivity, kMinPitchDeg, kMaxPitchDeg);
        });

        GM_MN.setMouseScrollCallback([this](double xoffset, double yoffset)
        {
            (void)xoffset;
            distance_ = std::clamp(distance_ - static_cast<float>(yoffset) * 6.0f, kMinDistance, kMaxDistance);
        });
    }

    void OrbitCamera::Detach()
    {
        if (!attached_)
        {
            return;
        }
        attached_ = false;

        // No-op lambdas rather than empty std::functions: GameManager invokes these
        // unconditionally, and calling an empty std::function throws.
        GM_MN.setMousePositionCallback([](double, double) {});
        GM_MN.setMouseScrollCallback([](double, double) {});

        INP_MN.SetDisableMouse(false);
        INP_MN.SetInactiveMouseInput();
    }

    void OrbitCamera::Update(const Ship &ship)
    {
        // UIManager switches the input source between mouse and keyboard every frame
        // and calls SetInactiveMouseInput()/SetDisableMouse(false) when it decides you
        // are on the keyboard -- which killed mouse-look the moment J/K/steering was
        // pressed. Re-assert capture here so the debug free-look survives that.
        INP_MN.SetActiveMouseInput();
        if (!INP_MN.GetIsDisableMouse())
        {
            INP_MN.SetDisableMouse(true);
        }

        const glm::vec3 target = ship.GetPosition();

        // Spherical offset around the ship. Yaw 0 sits behind it looking forward, so
        // the default view roughly matches the normal chase cam before you drag.
        const float yaw = glm::radians(yawDeg_);
        const float pitch = glm::radians(pitchDeg_);
        const float horizontal = std::cos(pitch) * distance_;

        const glm::vec3 offset(
            std::sin(yaw) * horizontal,
            std::sin(pitch) * distance_ * -1.0f,
            std::cos(yaw) * horizontal);

        CAM_MN.SetFocusPosition(target + offset, target);
    }
}
