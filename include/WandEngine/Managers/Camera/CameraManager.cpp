#include "CameraManager.hpp"

#include "../GameManager.hpp"
#include "../../Models/GameObject.hpp"
#include "../Body/BodyManager.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace WandEngine
{

    CameraManager::CameraManager() 
        : mode(CameraMode::CAMERA_2D)
    {
        Reset();
        camera3D = new Camera3D(glm::vec3(0.0f, 0.0f, 3.0f));
        activeMouseInput = true;
        position2D = glm::vec2(0.0f);
        baseWindowSize = glm::vec2(1920.0f, 1080.0f);
        Zoom = 1.0f;
    }

    glm::vec2 CameraManager::GetBaseWindowSize()
    {
        return baseWindowSize;
    }

    glm::vec2 CameraManager::GetPosition() const
    {
        glm::vec2 cam_area = GetCameraOrthoArea();

        float left   = position2D.x - (cam_area.x * 0.5f) / Zoom;
        float bottom = position2D.y - (cam_area.y * 0.5f) / Zoom;

        return glm::vec2(left, bottom);
    }


    glm::vec2 CameraManager::GetCameraOrthoArea() const
    {
        float baseAspectRatio = baseWindowSize.x / baseWindowSize.y;

        glm::vec2 screenSize = GameManager::GetInstance().GetWindowsSize();
        float aspectRatio = screenSize.x / screenSize.y ;

        float orthoWidth = baseWindowSize.x;
        float orthoHeight = baseWindowSize.y;

        if (aspectRatio > baseAspectRatio) {
            orthoWidth = baseWindowSize.y * aspectRatio;
        } else {
            orthoHeight = baseWindowSize.x / aspectRatio;
        }

        return glm::vec2(orthoWidth, orthoHeight);

    }


    void CameraManager::SetCameraMode(CameraMode newMode)
    {
        mode = newMode;
    }

    CameraMode CameraManager::GetCameraMode() const
    {
        return mode;
    }

    glm::mat4 CameraManager::GetViewMatrix()
    {
        if (mode == CameraMode::CAMERA_3D)
        {
            return camera3D->GetViewMatrix();
        }
        else
        {
            return glm::lookAt(glm::vec3(position2D, 1.0f), glm::vec3(position2D, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
    }

    glm::mat4 CameraManager::GetProjectionMatrix()
    {
        glm::vec2 cam_area = GetCameraOrthoArea();

        if (mode == CameraMode::CAMERA_3D)
        {
            glm::vec2 screenSize = GameManager::GetInstance().GetWindowsSize();
            float aspectRatio = screenSize.x / screenSize.y ;
            return camera3D->GetProjectionMatrix(aspectRatio);
        }
        else
        {
            //normal
            return glm::ortho(
                position2D.x - (cam_area.x * 0.5f) / Zoom,
                position2D.x + (cam_area.x * 0.5f) / Zoom,
                position2D.y + (cam_area.y * 0.5f) / Zoom,
                position2D.y - (cam_area.y * 0.5f) / Zoom,
                -1.0f, 1.0f
            );
            //center
            //return glm::ortho(position2D.x - orthoWidth * 0.5f, position2D.x + orthoWidth * 0.5f, position2D.y + orthoHeight * 0.5f, position2D.y - orthoHeight * 0.5f,  -1.0f, 1.0f);
        }
    }

    void CameraManager::ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        if (mode == CameraMode::CAMERA_3D)
        {
            camera3D->ProcessKeyboard(direction, deltaTime);
        }
    }

    void CameraManager::ProcessMouseMovement(double xoffset, double yoffset)
    {
        if(activeMouseInput)
        {
            float xpos = static_cast<float>(xoffset);
            float ypos = static_cast<float>(yoffset);
            if (mode == CameraMode::CAMERA_3D)
            {
                camera3D->ProcessMouseMovement(xpos, ypos);
            }
        }
    }

    void CameraManager::ProcessMouseScroll( double xoffset, double yoffset)
    {
        if(activeMouseInput)
        {
            float xpos = static_cast<float>(xoffset);
            float ypos = static_cast<float>(yoffset);
            if (mode == CameraMode::CAMERA_3D)
            {
                camera3D->ProcessMouseScroll(ypos);
            }
        }
    }

    void CameraManager::SetActiveMouseInput()
    {
        activeMouseInput = true;
    }

    void CameraManager::SetInactiveMouseInput()
    {
        activeMouseInput = false;
    }

    bool CameraManager::GetIsMouseInputActive()
    {
        return activeMouseInput;
    }


    void CameraManager::Reset()
    {
        if (mode == CameraMode::CAMERA_2D)
        {
            position2D = glm::vec2(0.0f);
        }
    }


    void CameraManager::SetFocusObject(GameObject *Obj)
    {
        if (mode == CameraMode::CAMERA_2D)
        {
            position2D.x = (Obj->GetPosition().x + Obj->GetSize().x / 2);
            position2D.y = (Obj->GetPosition().y + Obj->GetSize().y / 2);
        }
    }

    void CameraManager::SetFocusPosition(glm::vec2 NewPosition)
    {
        if (mode == CameraMode::CAMERA_2D)
        {
            position2D.x =  NewPosition.x;
            position2D.y =  NewPosition.y;
        }
    }

    glm::vec2 CameraManager::GetFocusPosition() const
    {
        return glm::vec2(position2D.x , position2D.y );
    }

    bool CameraManager::IsObjectInsideCameraArea(GameObject *Obj) const
    {
        glm::vec2 cam_area = GetCameraOrthoArea();

        float left   = position2D.x - (cam_area.x * 0.5f) / Zoom;
        float right  = position2D.x + (cam_area.x * 0.5f) / Zoom;
        float top    = position2D.y + (cam_area.y * 0.5f) / Zoom;
        float bottom = position2D.y - (cam_area.y * 0.5f) / Zoom;

        glm::vec2 objPos = Obj->GetPosition();
        glm::vec2 objSize = Obj->GetSize();

        return (objPos.x + objSize.x > left &&
                objPos.x < right &&
                objPos.y + objSize.y > bottom &&
                objPos.y < top);
    }


    CameraManager::~CameraManager()
    {
        delete camera3D;
    }

}
