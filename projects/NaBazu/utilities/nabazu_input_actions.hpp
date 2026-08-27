#ifndef NABAZU_INPUT_ACTIONS_HPP
#define NABAZU_INPUT_ACTIONS_HPP

#include <CosmicEngine/managers/input/input_manager.hpp>

#include <GLFW/glfw3.h>

#include <array>

namespace NaBazu
{
    struct NaBazuInputActionSpec
    {
        const char *actionName;
        CosmicEngine::InputActionBinding defaultBinding;
    };

    inline constexpr const char *kActionSteerLeft = "nabazu_steer_left";
    inline constexpr const char *kActionSteerRight = "nabazu_steer_right";
    inline constexpr const char *kActionSteerUp = "nabazu_steer_up";
    inline constexpr const char *kActionSteerDown = "nabazu_steer_down";
    inline constexpr const char *kActionFirePrimary = "nabazu_fire_primary";
    inline constexpr const char *kActionFireMissile = "nabazu_fire_missile";
    inline constexpr const char *kActionBrake = "nabazu_brake";
    inline constexpr const char *kActionToggleDebugBodies = "nabazu_toggle_debug_bodies";
    inline constexpr const char *kActionConfirm = "nabazu_confirm";
    inline constexpr const char *kActionToggleFullscreen = "nabazu_toggle_fullscreen";

    inline const std::array<NaBazuInputActionSpec, 10> &GetNaBazuInputActionSpecs()
    {
        static const std::array<NaBazuInputActionSpec, 10> specs = {{
            {kActionSteerLeft, {{GLFW_KEY_A, GLFW_KEY_LEFT}, {}, {}, {GLFW_GAMEPAD_AXIS_LEFT_X}}},
            {kActionSteerRight, {{GLFW_KEY_D, GLFW_KEY_RIGHT}, {}, {GLFW_GAMEPAD_AXIS_LEFT_X}, {}}},
            {kActionSteerUp, {{GLFW_KEY_W, GLFW_KEY_UP}, {}, {}, {GLFW_GAMEPAD_AXIS_LEFT_Y}}},
            {kActionSteerDown, {{GLFW_KEY_S, GLFW_KEY_DOWN}, {}, {GLFW_GAMEPAD_AXIS_LEFT_Y}, {}}},
            {kActionFirePrimary, {{GLFW_KEY_SPACE}, {GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER}, {}, {}}},
            {kActionFireMissile, {{GLFW_KEY_LEFT_SHIFT}, {GLFW_GAMEPAD_BUTTON_LEFT_BUMPER}, {}, {}}},
            {kActionBrake, {{GLFW_KEY_LEFT_CONTROL, GLFW_KEY_C}, {GLFW_GAMEPAD_BUTTON_X}, {}, {}}},
            {kActionToggleDebugBodies, {{GLFW_KEY_B}, {}, {}, {}}},
            {kActionConfirm, {{GLFW_KEY_ENTER}, {GLFW_GAMEPAD_BUTTON_A, GLFW_GAMEPAD_BUTTON_START}, {}, {}}},
            {kActionToggleFullscreen, {{GLFW_KEY_F11}, {}, {}, {}}}
        }};
        return specs;
    }

    inline void RegisterNaBazuInputActions()
    {
        auto &input = CosmicEngine::InputManager::GetInstance();
        for (const auto &spec : GetNaBazuInputActionSpecs())
        {
            input.RegisterAction(spec.actionName, spec.defaultBinding);
        }
    }
}

#endif
