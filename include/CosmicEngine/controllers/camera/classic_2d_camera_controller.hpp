#ifndef COSMIC_CLASSIC_2D_CAMERA_CONTROLLER_HPP
#define COSMIC_CLASSIC_2D_CAMERA_CONTROLLER_HPP

#include "camera_controller.hpp"
#include "../../managers/camera/camera_manager.hpp"
#include "../../managers/input/input_manager.hpp"

namespace CosmicEngine
{
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    class Classic2DCameraController : public ICameraController
    {
    public:
        Classic2DCameraController(
            CameraManager &camera = CameraManager::GetInstance(),
            InputManager &input = InputManager::GetInstance());

        void SetKeyboardMoveSpeed(float newKeyboardMoveSpeed);
        float GetKeyboardMoveSpeed() const;

        void Update(double deltaTime) override;

        static void ProcessKeyboard(CameraManager &camera, Camera_Movement direction, float deltaTime, float keyboardMoveSpeed = 1440.0f);
        static void ProcessMouseMovement(CameraManager &camera, float xoffset, float yoffset);
        static void ProcessMouseScroll(CameraManager &camera, float xoffset, float yoffset);

    private:
        CameraManager &camera;
        InputManager &input;
        float keyboardMoveSpeed;
    };
#endif
}

#endif