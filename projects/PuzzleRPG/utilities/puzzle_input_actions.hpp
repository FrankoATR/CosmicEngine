#ifndef PUZZLERPG_INPUT_ACTIONS_HPP
#define PUZZLERPG_INPUT_ACTIONS_HPP

#include <CosmicEngine/managers/input/input_manager.hpp>

#include <GLFW/glfw3.h>

#include <array>
#include <string_view>

namespace PuzzleRPG
{
    struct PuzzleInputActionSpec
    {
        const char *actionName;
        const char *displayName;
        CosmicEngine::InputActionBinding defaultBinding;
    };

    inline constexpr const char *kActionMoveUp = "puzzle_move_up";
    inline constexpr const char *kActionMoveDown = "puzzle_move_down";
    inline constexpr const char *kActionMoveLeft = "puzzle_move_left";
    inline constexpr const char *kActionMoveRight = "puzzle_move_right";
    inline constexpr const char *kActionInteract = "puzzle_interact";
    inline constexpr const char *kActionUiSubmit = "ui_submit";
    inline constexpr const char *kActionPause = "puzzle_pause";

    inline const std::array<PuzzleInputActionSpec, 7> &GetPuzzleInputActionSpecs()
    {
        static const std::array<PuzzleInputActionSpec, 7> specs = {{
            {kActionMoveUp, "Mover arriba", {{GLFW_KEY_W, GLFW_KEY_UP}, {GLFW_GAMEPAD_BUTTON_DPAD_UP}, {}, {GLFW_GAMEPAD_AXIS_LEFT_Y}}},
            {kActionMoveDown, "Mover abajo", {{GLFW_KEY_S, GLFW_KEY_DOWN}, {GLFW_GAMEPAD_BUTTON_DPAD_DOWN}, {GLFW_GAMEPAD_AXIS_LEFT_Y}, {}}},
            {kActionMoveLeft, "Mover izquierda", {{GLFW_KEY_A, GLFW_KEY_LEFT}, {GLFW_GAMEPAD_BUTTON_DPAD_LEFT}, {}, {GLFW_GAMEPAD_AXIS_LEFT_X}}},
            {kActionMoveRight, "Mover derecha", {{GLFW_KEY_D, GLFW_KEY_RIGHT}, {GLFW_GAMEPAD_BUTTON_DPAD_RIGHT}, {GLFW_GAMEPAD_AXIS_LEFT_X}, {}}},
            {kActionInteract, "Interactuar", {{GLFW_KEY_E}, {GLFW_GAMEPAD_BUTTON_A}, {}, {}}},
            {kActionUiSubmit, "Aceptar UI", {{GLFW_KEY_ENTER}, {GLFW_GAMEPAD_BUTTON_A}, {}, {}}},
            {kActionPause, "Pausa", {{GLFW_KEY_ESCAPE}, {GLFW_GAMEPAD_BUTTON_START, GLFW_GAMEPAD_BUTTON_B}, {}, {}}}
        }};
        return specs;
    }

    inline void RegisterPuzzleInputActions()
    {
        auto &input = CosmicEngine::InputManager::GetInstance();
        for (const auto &spec : GetPuzzleInputActionSpecs())
        {
            input.RegisterAction(spec.actionName, spec.defaultBinding);
        }
    }
}

#endif
