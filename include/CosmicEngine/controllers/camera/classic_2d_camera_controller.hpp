#ifndef COSMIC_CLASSIC_2D_CAMERA_CONTROLLER_HPP
#define COSMIC_CLASSIC_2D_CAMERA_CONTROLLER_HPP

/**
 * @file classic_2d_camera_controller.hpp
 * @brief Declares the classic 2D camera controller.
 */

#include "camera_controller.hpp"
#include "../../managers/camera/camera_manager.hpp"
#include "../../managers/input/input_manager.hpp"

namespace CosmicEngine
{
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    /**
     * @brief Default keyboard and mouse controller for the 2D camera manager.
     *
     * Classic2DCameraController binds WASD/arrow keys and mouse scroll to
     * CameraManager focus and zoom.  Create it once and call Update() every
     * frame from your scene.
     *
     * @par Example
     * @code
     * auto controller = std::make_unique<CosmicEngine::Classic2DCameraController>();
     *
     * // Inside Scene::update():
     * controller->Update(deltaTime);
     * @endcode
     */
    class Classic2DCameraController : public ICameraController
    {
    public:
        /** @brief Creates a classic 2D camera controller. */
        Classic2DCameraController(
            CameraManager &camera = CameraManager::GetInstance(),
            InputManager &input = InputManager::GetInstance());

        /** @brief Sets keyboard movement speed. */
        void SetKeyboardMoveSpeed(float newKeyboardMoveSpeed);
        /** @brief Returns keyboard movement speed. */
        float GetKeyboardMoveSpeed() const;

        /** @brief Updates keyboard-driven camera movement. */
        void Update(double deltaTime) override;

        /** @brief Applies a keyboard movement step to a 2D camera. */
        static void ProcessKeyboard(CameraManager &camera, Camera_Movement direction, float deltaTime, float keyboardMoveSpeed = 1440.0f);
        /** @brief Applies mouse movement to a 2D camera. */
        static void ProcessMouseMovement(CameraManager &camera, float xoffset, float yoffset);
        /** @brief Applies mouse scroll to a 2D camera. */
        static void ProcessMouseScroll(CameraManager &camera, float xoffset, float yoffset);

    private:
        CameraManager &camera;
        InputManager &input;
        float keyboardMoveSpeed;
    };
#endif
}

#endif