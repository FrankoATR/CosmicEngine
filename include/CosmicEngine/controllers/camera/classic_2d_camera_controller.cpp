#include "classic_2d_camera_controller.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace CosmicEngine
{
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    Classic2DCameraController::Classic2DCameraController(CameraManager &camera, InputManager &input)
        : camera(camera), input(input), keyboardMoveSpeed(1440.0f)
    {
    }

    void Classic2DCameraController::SetKeyboardMoveSpeed(float newKeyboardMoveSpeed)
    {
        keyboardMoveSpeed = newKeyboardMoveSpeed;
    }

    float Classic2DCameraController::GetKeyboardMoveSpeed() const
    {
        return keyboardMoveSpeed;
    }

    void Classic2DCameraController::Update(double deltaTime)
    {
        float frameDelta = static_cast<float>(deltaTime);
        const float horizontalAxis = input.GetActionAxis("camera_move_left", "camera_move_right");
        const float verticalAxis = input.GetActionAxis("camera_move_up", "camera_move_down");

        if (horizontalAxis < 0.0f)
        {
            ProcessKeyboard(camera, CosmicEngine::LEFT, frameDelta * -horizontalAxis, keyboardMoveSpeed);
        }

        if (horizontalAxis > 0.0f)
        {
            ProcessKeyboard(camera, CosmicEngine::RIGHT, frameDelta * horizontalAxis, keyboardMoveSpeed);
        }

        if (verticalAxis < 0.0f)
        {
            ProcessKeyboard(camera, CosmicEngine::UP, frameDelta * -verticalAxis, keyboardMoveSpeed);
        }

        if (verticalAxis > 0.0f)
        {
            ProcessKeyboard(camera, CosmicEngine::DOWN, frameDelta * verticalAxis, keyboardMoveSpeed);
        }
    }

    void Classic2DCameraController::ProcessKeyboard(CameraManager &camera, Camera_Movement direction, float deltaTime, float keyboardMoveSpeed)
    {
        float velocity = keyboardMoveSpeed * deltaTime;

        if (direction == LEFT)
        {
            camera.MoveFocus(glm::vec2(-velocity, 0.0f));
        }
        else if (direction == RIGHT)
        {
            camera.MoveFocus(glm::vec2(velocity, 0.0f));
        }
        else if (direction == UP)
        {
            camera.MoveFocus(glm::vec2(0.0f, -velocity));
        }
        else if (direction == DOWN)
        {
            camera.MoveFocus(glm::vec2(0.0f, velocity));
        }
    }

    void Classic2DCameraController::ProcessMouseMovement(CameraManager &camera, float xoffset, float yoffset)
    {
        (void)camera;
        (void)xoffset;
        (void)yoffset;
    }

    void Classic2DCameraController::ProcessMouseScroll(CameraManager &camera, float xoffset, float yoffset)
    {
        (void)xoffset;
        camera.AdjustZoom(-yoffset * 0.1f);
    }
#endif
}