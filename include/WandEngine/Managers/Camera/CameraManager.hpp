#ifndef CAMERAMANAGER_HPP
#define CAMERAMANAGER_HPP

#include "../../Utils/Configurations.hpp"
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace WandEngine
{

    class Object;

    #if defined(GAME_2D_CONFIGURATION)


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

            void Classic2DProcessKeyboard(Camera_Movement direction, float deltaTime); // Maybe unnecesary???
            void Classic2DProcessMouseMovement(float xoffset, float yoffset); // Maybe unnecesary???
            void Classic2DProcessMouseScroll(float xoffset, float yoffset); // Maybe unnecesary or for zoom???

            void SetActiveMouseInput();
            void SetInactiveMouseInput();
            bool GetIsMouseInputActive();

            void Reset();
        };


    #elif defined(GAME_3D_CONFIGURATION)

        enum Camera_Movement {
            FORWARD,
            BACKWARD,
            LEFT,
            RIGHT,
            UP,
            DOWN
        };

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
            float lastX;
            float lastY;
            bool firstMouse;

            GLboolean constrainPitch;

            void updateCameraVectors();

            glm::vec2 GetCameraOrthoArea() const;

        public:
            static CameraManager &GetInstance();
            
            void Init(glm::vec2 baseSize);

            glm::vec2 GetBaseWindowSize() const;

            glm::mat4 GetViewMatrix() const;
            glm::mat4 GetProjectionMatrix() const;
            glm::mat4 Get_UI_ProjectionMatrix() const;

            float GetMovementSpeed() const;
            void SetMovementSpeed(float newMovementSpeed);
            
            glm::vec3 GetPosition() const;
            glm::vec3 GetViewDirection() const;
            
            void SetFocusObject(glm::vec3 from, Object *ObjToLook, float xoffset = 0.0f, float yoffset = 0.0f);
            void SetFocusPosition(glm::vec3 from, glm::vec3 lookingAt);

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
