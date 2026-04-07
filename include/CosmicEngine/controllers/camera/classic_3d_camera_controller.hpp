#ifndef COSMIC_CLASSIC_3D_CAMERA_CONTROLLER_HPP
#define COSMIC_CLASSIC_3D_CAMERA_CONTROLLER_HPP

#include "camera_controller.hpp"
#include "../../managers/camera/camera_manager.hpp"
#include "../../managers/input/input_manager.hpp"

namespace CosmicEngine
{
    class GameManager;

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    class Classic3DCameraController : public ICameraController
    {
    public:
        Classic3DCameraController(
            CameraManager &camera = CameraManager::GetInstance(),
            InputManager &input = InputManager::GetInstance());

        void Update(double deltaTime) override;
        void OnMouseMove(double xpos, double ypos) override;
        void OnMouseScroll(double xoffset, double yoffset) override;

        void Attach(GameManager &gameManager);
        void Detach(GameManager &gameManager);
        void SetMouseCaptureEnabled(bool enabled);
        bool IsMouseCaptureEnabled() const;

        static void ProcessKeyboard(CameraManager &camera, Camera_Movement direction, float deltaTime);
        static void ProcessMouseMovement(CameraManager &camera, InputManager &input, float xpos, float ypos);
        static void ProcessMouseScroll(CameraManager &camera, InputManager &input, float xoffset, float yoffset);

    private:
        CameraManager &camera;
        InputManager &input;
        bool mouseCaptureEnabled;
    };
#endif
}

#endif