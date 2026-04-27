#ifndef COSMIC_CLASSIC_3D_CAMERA_CONTROLLER_HPP
#define COSMIC_CLASSIC_3D_CAMERA_CONTROLLER_HPP

/**
 * @file classic_3d_camera_controller.hpp
 * @brief Declares the classic 3D camera controller.
 */

#include "camera_controller.hpp"
#include "../../managers/camera/camera_manager.hpp"
#include "../../managers/input/input_manager.hpp"

namespace CosmicEngine
{
    class GameManager;

#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    /**
     * @brief Default first-person style controller for the 3D camera manager.
     *
     * Classic3DCameraController binds WASD to movement and mouse look to
     * yaw/pitch.  Call Attach(gameManager) once so the controller can
     * install mouse callbacks automatically.
     *
     * @par Example
     * @code
     * auto ctrl = std::make_unique<CosmicEngine::Classic3DCameraController>();
     * ctrl->Attach(GM_MN);
     * ctrl->SetMouseCaptureEnabled(true);
     * CAM_MN.SetMovementSpeed(28.0f);
     *
     * // Inside Scene::update():
     * ctrl->Update(deltaTime);
     * @endcode
     */
    class Classic3DCameraController : public ICameraController
    {
    public:
        /** @brief Creates a classic 3D camera controller. */
        Classic3DCameraController(
            CameraManager &camera = CameraManager::GetInstance(),
            InputManager &input = InputManager::GetInstance());

        /** @brief Updates keyboard-driven camera movement. */
        void Update(double deltaTime) override;
        /** @brief Handles mouse-look movement. */
        void OnMouseMove(double xpos, double ypos) override;
        /** @brief Handles mouse scroll zoom. */
        void OnMouseScroll(double xoffset, double yoffset) override;

        /** @brief Attaches the controller to a game manager callback flow. */
        void Attach(GameManager &gameManager);
        /** @brief Detaches the controller from a game manager callback flow. */
        void Detach(GameManager &gameManager);
        /** @brief Enables or disables mouse capture mode. */
        void SetMouseCaptureEnabled(bool enabled);
        /**
         * @brief Returns whether mouse capture mode is enabled.
         * @return The whether mouse capture mode is enabled.
         */
        bool IsMouseCaptureEnabled() const;

        /** @brief Applies a keyboard movement step to a 3D camera. */
        static void ProcessKeyboard(CameraManager &camera, Camera_Movement direction, float deltaTime);
        /** @brief Applies relative mouse movement to a 3D camera. */
        static void ProcessMouseMovement(CameraManager &camera, InputManager &input, float xpos, float ypos);
        /** @brief Applies mouse scroll to a 3D camera. */
        static void ProcessMouseScroll(CameraManager &camera, InputManager &input, float xoffset, float yoffset);

    private:
        CameraManager &camera;
        InputManager &input;
        bool mouseCaptureEnabled;
    };
#endif
}

#endif
