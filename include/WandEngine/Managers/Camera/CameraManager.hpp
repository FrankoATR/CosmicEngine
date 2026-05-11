#ifndef CAMERAMANAGER_HPP
#define CAMERAMANAGER_HPP
#include "../../Models/Camera/3DView/Camera3D.hpp"

#include "../../Utils/Configurations.hpp"
#include <glm/glm.hpp>

namespace WandEngine
{

    #if defined(GAME_2D_CONFIGURATION)

        class Object;

        class CameraManager
        {
        private:
            CameraManager();
            ~CameraManager();
            CameraManager(const CameraManager &) = delete;
            CameraManager &operator=(const CameraManager &) = delete;

            glm::vec2 baseWindowSize;

            bool activeMouseInput;
            glm::vec2 position2D;
            float zoom;

            glm::vec2 GetGridPosition() const;
            void SetGridPosition(glm::vec2 newPosition);

            glm::vec2 GetCameraOrthoArea() const;

        public:
            static CameraManager &GetInstance();

            void Init(glm::vec2 baseSize);

            glm::vec2 GetBaseWindowSize() const;

            glm::mat4 GetViewMatrix() const;
            glm::mat4 GetProjectionMatrix() const;
            glm::mat4 Get_UI_ProjectionMatrix() const;

            // Métodos 2D
            glm::vec2 GetPosition() const;
            void SetFocusObject(Object *Obj, float xoffset = 0.0f, float yoffset = 0.0f);
            void SetFocusPosition(glm::vec2 NewPosition);
            glm::vec2 GetFocusPosition() const;
            bool IsObjectInsideCameraArea(Object *Obj);

            // Métodos 3D
            void ProcessMouseMovement(double xoffset, double yoffset);
            void ProcessMouseScroll(double xoffset, double yoffset);

            void SetActiveMouseInput();
            void SetInactiveMouseInput();
            bool GetIsMouseInputActive();

            void Reset();
        };


    #elif defined(GAME_3D_CONFIGURATION)

        class Object;

        class CameraManager
        {
        private:
            CameraManager();
            ~CameraManager();
            CameraManager(const CameraManager &) = delete;
            CameraManager &operator=(const CameraManager &) = delete;

            glm::vec2 baseWindowSize;
            float baseAspectRatio;
            bool activeMouseInput;

            glm::vec3 position;
            glm::vec3 frontBody;
            glm::vec3 up;
            glm::vec3 right;
            glm::vec3 worldUp;
            float yaw;
            float pitch;
            float movementSpeed;
            float mouseSensitivity;
            float zoom;
            float lastX;
            float lastY;
            bool firstMouse;

            GLboolean constrainPitch;

            void updateCameraVectors();

            glm::vec2 GetCameraOrthoArea() const;

        public:
            static CameraManager &GetInstance();
            
            glm::vec3 front;

            void Init(glm::vec2 baseSize);

            glm::vec2 GetBaseWindowSize() const;

            glm::mat4 GetViewMatrix() const;
            glm::mat4 GetProjectionMatrix() const;
            glm::mat4 Get_UI_ProjectionMatrix() const;

            // Métodos 2D
            glm::vec3 GetPosition() const;
            void SetFocusObject(glm::vec3 from, Object *ObjToLook, float xoffset = 0.0f, float yoffset = 0.0f);
            void SetFocusPosition(glm::vec3 from, glm::vec3 lookingAt);
            glm::vec3 GetViewDirection() const;

            // Métodos 3D
            void Classic3DProcessKeyboard(Camera_Movement direction, float deltaTime);
            void Classic3DProcessMouseMovement(float xoffset, float yoffset);
            void Classic3DProcessMouseScroll(float xoffset, float yoffset);

            void SetActiveMouseInput();
            void SetInactiveMouseInput();
            bool GetIsMouseInputActive();

            void Reset();
        };



    #else
        #error "[CameraManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif


}

#endif // CAMERAMANAGER_HPP
