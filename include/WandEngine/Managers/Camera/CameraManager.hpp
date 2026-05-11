#ifndef CAMERAMANAGER_HPP
#define CAMERAMANAGER_HPP

#include <glm/glm.hpp>
#include "../../Models/Camera/3DView/Camera3D.hpp"

namespace WandEngine
{

    enum class CameraMode
    {
        CAMERA_2D,
        CAMERA_3D
    };

    class GameObject;

    class CameraManager
    {
    private:
        CameraManager();
        ~CameraManager();
        CameraManager(const CameraManager &) = delete;
        CameraManager &operator=(const CameraManager &) = delete;
        
        glm::vec2 baseWindowSize;

        bool activeMouseInput;
        CameraMode mode;
        Camera3D* camera3D;
        glm::vec2 position2D;
        float Zoom;

        glm::vec2 GetGridPosition() const;
        void SetGridPosition(glm::vec2 NewPosition);

        glm::vec2 GetCameraOrthoArea() const;

    public:
        static CameraManager &GetInstance();

        void Init(glm::vec2 baseSize);

        glm::vec2 GetBaseWindowSize() const;

        void SetCameraMode(CameraMode newMode);
        CameraMode GetCameraMode() const;

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const;
        glm::mat4 Get_UI_ProjectionMatrix() const;

        // Métodos 2D
        glm::vec2 GetPosition() const;
        void SetFocusObject(GameObject *Obj, float xoffset = 0.0f, float yoffset = 0.0f);
        void SetFocusPosition(glm::vec2 NewPosition);
        glm::vec2 GetFocusPosition() const;
        bool IsObjectInsideCameraArea(GameObject *Obj) const;

        // Métodos 3D
        void ProcessKeyboard(Camera_Movement direction, float deltaTime);
        void ProcessMouseMovement(double xoffset, double yoffset);
        void ProcessMouseScroll( double xoffset, double yoffset);

        void SetActiveMouseInput();
        void SetInactiveMouseInput();
        bool GetIsMouseInputActive();
        
        void Reset();
    };

}

#endif // CAMERAMANAGER_HPP
