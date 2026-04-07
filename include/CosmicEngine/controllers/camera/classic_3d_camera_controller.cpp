#include "classic_3d_camera_controller.hpp"

#include "../../managers/game_manager.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace CosmicEngine
{
#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    Classic3DCameraController::Classic3DCameraController(CameraManager &camera, InputManager &input)
        : camera(camera), input(input), mouseCaptureEnabled(true)
    {
    }

    void Classic3DCameraController::Update(double deltaTime)
    {
        if (!mouseCaptureEnabled)
        {
            return;
        }

        float frameDelta = static_cast<float>(deltaTime);

        if (input.IsKeyPressed(GLFW_KEY_W, CosmicEngine::KeyRelease))
        {
            ProcessKeyboard(camera, CosmicEngine::FORWARD, frameDelta);
        }

        if (input.IsKeyPressed(GLFW_KEY_S, CosmicEngine::KeyRelease))
        {
            ProcessKeyboard(camera, CosmicEngine::BACKWARD, frameDelta);
        }

        if (input.IsKeyPressed(GLFW_KEY_A, CosmicEngine::KeyRelease))
        {
            ProcessKeyboard(camera, CosmicEngine::LEFT, frameDelta);
        }

        if (input.IsKeyPressed(GLFW_KEY_D, CosmicEngine::KeyRelease))
        {
            ProcessKeyboard(camera, CosmicEngine::RIGHT, frameDelta);
        }

        if (input.IsKeyPressed(GLFW_KEY_SPACE, CosmicEngine::KeyRelease))
        {
            ProcessKeyboard(camera, CosmicEngine::UP, frameDelta);
        }

        if (input.IsKeyPressed(GLFW_KEY_LEFT_SHIFT, CosmicEngine::KeyRelease))
        {
            ProcessKeyboard(camera, CosmicEngine::DOWN, frameDelta);
        }
    }

    void Classic3DCameraController::OnMouseMove(double xpos, double ypos)
    {
        if (!mouseCaptureEnabled)
        {
            return;
        }

        ProcessMouseMovement(camera, input, static_cast<float>(xpos), static_cast<float>(ypos));
    }

    void Classic3DCameraController::OnMouseScroll(double xoffset, double yoffset)
    {
        if (!mouseCaptureEnabled)
        {
            return;
        }

        ProcessMouseScroll(camera, input, static_cast<float>(xoffset), static_cast<float>(yoffset));
    }

    void Classic3DCameraController::Attach(GameManager &gameManager)
    {
        gameManager.setMousePositionCallback([this](double xpos, double ypos)
        {
            OnMouseMove(xpos, ypos);
        });

        gameManager.setMouseScrollCallback([this](double xoffset, double yoffset)
        {
            OnMouseScroll(xoffset, yoffset);
        });
    }

    void Classic3DCameraController::Detach(GameManager &gameManager)
    {
        gameManager.setMousePositionCallback({});
        gameManager.setMouseScrollCallback({});
    }

    void Classic3DCameraController::SetMouseCaptureEnabled(bool enabled)
    {
        mouseCaptureEnabled = enabled;

        input.SetInactiveMouseInput();
        input.SetDisableMouse(enabled);

        if (enabled)
        {
            input.ResetMouseLookReference();
            input.SetActiveMouseInput();
        }
    }

    bool Classic3DCameraController::IsMouseCaptureEnabled() const
    {
        return mouseCaptureEnabled;
    }

    void Classic3DCameraController::ProcessKeyboard(CameraManager &camera, Camera_Movement direction, float deltaTime)
    {
        float velocity = camera.GetMovementSpeed() * deltaTime;

        if (direction == FORWARD)
        {
            camera.Move(camera.GetBodyDirection() * velocity);
        }
        else if (direction == BACKWARD)
        {
            camera.Move(-camera.GetBodyDirection() * velocity);
        }
        else if (direction == LEFT)
        {
            camera.Move(-camera.GetRightDirection() * velocity);
        }
        else if (direction == RIGHT)
        {
            camera.Move(camera.GetRightDirection() * velocity);
        }
        else if (direction == UP)
        {
            camera.Move(glm::vec3(0.0f, velocity, 0.0f));
        }
        else if (direction == DOWN)
        {
            camera.Move(glm::vec3(0.0f, -velocity, 0.0f));
        }
    }

    void Classic3DCameraController::ProcessMouseMovement(CameraManager &camera, InputManager &input, float xpos, float ypos)
    {
        float xoffset = 0.0f;
        float yoffset = 0.0f;

        if (!input.TryGetMouseLookDelta(xpos, ypos, xoffset, yoffset))
        {
            return;
        }

        camera.RotateBy(xoffset * camera.GetMouseSensitivity(), yoffset * camera.GetMouseSensitivity());
    }

    void Classic3DCameraController::ProcessMouseScroll(CameraManager &camera, InputManager &input, float xoffset, float yoffset)
    {
        (void)xoffset;

        if (!input.GetIsMouseInputActive())
        {
            return;
        }

        camera.AdjustZoom(-yoffset);
    }
#endif
}