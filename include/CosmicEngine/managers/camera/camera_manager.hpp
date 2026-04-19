#ifndef COSMIC_CAMERAMANAGER_HPP
#define COSMIC_CAMERAMANAGER_HPP

/**
 * @file camera_manager.hpp
 * @brief Declares the engine camera manager for 2D and 3D runtime configurations.
 */

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace CosmicEngine
{

    class Object;

    #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION

        /**
         * @brief Supported movement directions for the classic 2D camera controller.
         */
        enum Camera_Movement {
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

        /**
         * @brief Manages 2D camera state, projection, and optional controller-like movement helpers.
         *
         * In 2D mode the camera tracks a focus position and an orthographic zoom level.
         * Use SetFocusPosition() to center the view and AdjustZoom() to scale it.
         * For keyboard/mouse control, pair it with a Classic2DCameraController.
         *
         * @par Example — basic 2D camera setup
         * @code
         * CAM_MN.SetFocusPosition(glm::vec2(0.0f));
         *
         * // Create a camera controller and update it each frame:
         * auto controller = std::make_unique<CosmicEngine::Classic2DCameraController>();
         * controller->Update(deltaTime);
         * @endcode
         */
        class CameraManager
        {
        private:
            CameraManager();
            ~CameraManager();
            CameraManager(const CameraManager &) = delete;
            CameraManager &operator=(const CameraManager &) = delete;

            glm::vec2 baseWindowSize;

            glm::vec2 position2D;
            float zoom;

            glm::vec2 GetGridPosition() const;
            void SetGridPosition(glm::vec2 newPosition);

            glm::vec2 GetCameraOrthoArea() const;

        public:
            /** @brief Returns the singleton instance of the 2D camera manager. */
            static CameraManager &GetInstance();

            /**
             * @brief Initializes the camera manager using the provided virtual base size.
             * @param baseSize Base window size used for projection and UI calculations.
             */
            void init(glm::vec2 baseSize);

            /** @brief Returns the configured base window size. */
            glm::vec2 GetBaseWindowSize() const;

            /** @brief Returns the current 2D view matrix. */
            glm::mat4 GetViewMatrix() const;
            /** @brief Returns the current 2D projection matrix. */
            glm::mat4 GetProjectionMatrix() const;
            /** @brief Returns the projection matrix used for UI rendering. */
            glm::mat4 Get_UI_ProjectionMatrix() const;

            /** @brief Returns the current camera position. */
            glm::vec2 GetPosition() const;
            /**
             * @brief Focuses the camera on the provided object.
             * @param Obj Target object.
             * @param xoffset Horizontal offset applied to the focus point.
             * @param yoffset Vertical offset applied to the focus point.
             */
            void SetFocusObject(Object *Obj, float xoffset = 0.0f, float yoffset = 0.0f);
            /**
             * @brief Focuses the camera on the provided world position.
             * @param NewPosition Target focus position.
             */
            void SetFocusPosition(glm::vec2 NewPosition);
            /** @brief Returns the current camera focus position. */
            glm::vec2 GetFocusPosition() const;
            /**
             * @brief Moves the current focus point by an offset.
             * @param offset Offset to apply.
             */
            void MoveFocus(const glm::vec2 &offset);
            /** @brief Returns the current orthographic zoom value. */
            float GetZoom() const;
            /**
             * @brief Sets the orthographic zoom value.
             * @param newZoom New zoom amount.
             */
            void SetZoom(float newZoom);
            /**
             * @brief Adjusts the current zoom by an incremental offset.
             * @param zoomOffset Zoom increment.
             */
            void AdjustZoom(float zoomOffset);
            /**
             * @brief Checks whether an object overlaps the current visible camera area.
             * @param Obj Object to test.
             * @return True when the object is inside the camera area.
             */
            bool IsObjectInsideCameraArea(Object *Obj);

            /** @brief Applies classic 2D keyboard movement to the camera focus. */
            void Classic2DProcessKeyboard(Camera_Movement direction, float deltaTime);
            /** @brief Applies mouse movement to the classic 2D camera workflow. */
            void Classic2DProcessMouseMovement(float xoffset, float yoffset);
            /** @brief Applies mouse scroll input to the classic 2D camera workflow. */
            void Classic2DProcessMouseScroll(float xoffset, float yoffset);

            /** @brief Enables mouse-driven camera input. */
            void SetActiveMouseInput();
            /** @brief Disables mouse-driven camera input. */
            void SetInactiveMouseInput();
            /** @brief Returns whether mouse-driven camera input is currently enabled. */
            bool GetIsMouseInputActive();

            /** @brief Restores the 2D camera state to its default values. */
            void reset();
        };


    #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

        /**
         * @brief Supported movement directions for the classic 3D camera controller.
         */
        enum Camera_Movement {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

        /**
         * @brief Manages 3D camera transform, projection, and first-person style movement helpers.
         *
         * In 3D mode the camera maintains position, front, up, and right vectors with
         * yaw/pitch angles.  SetFocusPosition(pos, lookTarget) points the camera from
         * \e pos toward \e lookTarget.  Pair it with a Classic3DCameraController for
         * first-person keyboard/mouse control.
         *
         * @par Example — fixed 3D camera overlooking an arena
         * @code
         * glm::vec3 arenaCenter = kArenaPosition + kArenaSize * 0.5f;
         * glm::vec3 cameraPos   = arenaCenter + glm::vec3(0, 50, 50);
         * glm::vec3 lookTarget  = arenaCenter + glm::vec3(0, -20, 0);
         *
         * CAM_MN.reset();
         * CAM_MN.SetZoom(100.0f);
         * CAM_MN.SetFocusPosition(cameraPos, lookTarget);
         * CAM_MN.SetMovementSpeed(28.0f);
         * @endcode
         */
        class CameraManager
        {
        private:
            CameraManager();
            ~CameraManager();
            CameraManager(const CameraManager &) = delete;
            CameraManager &operator=(const CameraManager &) = delete;

            glm::vec2 baseWindowSize;
            float baseAspectRatio;
            glm::vec3 position;
            glm::vec3 front;
            glm::vec3 frontBody;
            glm::vec3 up;
            glm::vec3 right;
            glm::vec3 worldUp;
            float yaw;
            float pitch;
            float movementSpeed;
            float mouseSensitivity;
            float zoom;

            GLboolean constrainPitch;

            void updateCameraVectors();

            glm::vec2 GetCameraOrthoArea() const;

        public:
            /** @brief Returns the singleton instance of the 3D camera manager. */
            static CameraManager &GetInstance();
            
            /**
             * @brief Initializes the camera manager using the provided virtual base size.
             * @param baseSize Base window size used for projection and UI calculations.
             */
            void init(glm::vec2 baseSize);

            /** @brief Returns the configured base window size. */
            glm::vec2 GetBaseWindowSize() const;

            /** @brief Returns the current 3D view matrix. */
            glm::mat4 GetViewMatrix() const;
            /** @brief Returns the current 3D projection matrix. */
            glm::mat4 GetProjectionMatrix() const;
            /** @brief Returns the projection matrix used for UI rendering in 3D mode. */
            glm::mat4 Get_UI_ProjectionMatrix() const;

            /** @brief Returns the current camera movement speed. */
            float GetMovementSpeed() const;
            /** @brief Sets the camera movement speed. */
            void SetMovementSpeed(float newMovementSpeed);
            /** @brief Returns the current camera mouse sensitivity. */
            float GetMouseSensitivity() const;
            /** @brief Sets the camera mouse sensitivity. */
            void SetMouseSensitivity(float newMouseSensitivity);
            /** @brief Returns the current perspective zoom or field-of-view value. */
            float GetZoom() const;
            /** @brief Sets the current perspective zoom or field-of-view value. */
            void SetZoom(float newZoom);
            /** @brief Adjusts the current zoom by an incremental offset. */
            void AdjustZoom(float zoomOffset);
            
            /** @brief Returns the current camera position. */
            glm::vec3 GetPosition() const;
            /** @brief Returns the camera forward direction. */
            glm::vec3 GetViewDirection() const;
            /** @brief Returns the horizontal-facing body direction. */
            glm::vec3 GetBodyDirection() const;
            /** @brief Returns the camera right direction. */
            glm::vec3 GetRightDirection() const;
            /**
             * @brief Moves the camera by a world-space offset.
             * @param offset Translation offset.
             */
            void Move(const glm::vec3 &offset);
            /**
             * @brief Rotates the camera using yaw and pitch offsets.
             * @param yawOffset Horizontal rotation increment.
             * @param pitchOffset Vertical rotation increment.
             */
            void RotateBy(float yawOffset, float pitchOffset);
            
            /**
             * @brief Places the camera at a position and orients it toward an object.
             * @param from New camera position.
             * @param ObjToLook Target object.
             * @param xoffset Horizontal offset applied to the target point.
             * @param yoffset Vertical offset applied to the target point.
             */
            void SetFocusObject(glm::vec3 from, Object *ObjToLook, float xoffset = 0.0f, float yoffset = 0.0f);
            /**
             * @brief Places the camera at a position and orients it toward a target point.
             * @param from New camera position.
             * @param lookingAt Target world position.
             */
            void SetFocusPosition(glm::vec3 from, glm::vec3 lookingAt);

            /** @brief Applies classic 3D keyboard movement to the camera. */
            void Classic3DProcessKeyboard(Camera_Movement direction, float deltaTime);
            /** @brief Applies classic mouse-look rotation to the camera. */
            void Classic3DProcessMouseMovement(float xoffset, float yoffset);
            /** @brief Applies mouse scroll zoom to the camera. */
            void Classic3DProcessMouseScroll(float xoffset, float yoffset);
            /** @brief Resets the mouse-look reference used by the input manager. */
            void ResetMouseLookReference();

            /** @brief Enables mouse-driven camera input. */
            void SetActiveMouseInput();
            /** @brief Disables mouse-driven camera input. */
            void SetInactiveMouseInput();
            /** @brief Returns whether mouse-driven camera input is currently enabled. */
            bool GetIsMouseInputActive();

            /** @brief Restores the 3D camera state to its default values. */
            void reset();
        };



    #else
        #error "[CameraManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif


}

#endif // COSMIC_CAMERAMANAGER_HPP
