#include "camera_manager.hpp"

#include "../game_manager.hpp"
#include "../input/input_manager.hpp"
#include "../../models/object/object.hpp"
#include "../body/body_manager.hpp"

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    #include "../../controllers/camera/classic_2d_camera_controller.hpp"
#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    #include "../../controllers/camera/classic_3d_camera_controller.hpp"
#endif

#include <glm/gtc/matrix_transform.hpp>
#include "../../utils/log.hpp"


namespace CosmicEngine
{

    CameraManager &CameraManager::GetInstance()
    {
        static CameraManager instance;
        return instance;
    }

    CameraManager::CameraManager()
    {
        RUNTIME_INFO("Camera manager created");
    }

    CameraManager::~CameraManager()
    {
        RUNTIME_INFO("Camera manager destroyed");
    }

    glm::vec2 CameraManager::GetBaseWindowSize() const
    {
        return baseWindowSize;
    }

    void CameraManager::SetActiveMouseInput() //TODO: on inputManager
    {
        InputManager::GetInstance().SetActiveMouseInput();
    }

    void CameraManager::SetInactiveMouseInput() //TODO: on inputManager
    {
        InputManager::GetInstance().SetInactiveMouseInput();
    }

    bool CameraManager::GetIsMouseInputActive() //TODO: on inputManager
    {
        return InputManager::GetInstance().GetIsMouseInputActive();
    }

    glm::mat4 CameraManager::Get_UI_ProjectionMatrix() const
    {
        return glm::ortho(
            0.0f, baseWindowSize.x,
            baseWindowSize.y, 0.0f,
            -1.0f, 1.0f
        );
    }

    #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION

        void CameraManager::init(glm::vec2 baseSize)
        {
            this->position2D = glm::vec2(0.0f);
            this->baseWindowSize = baseSize;
            this->zoom = 1.0f;

            reset();

            RUNTIME_INFO("Camera manager initialized");
        }

        glm::vec2 CameraManager::GetPosition() const
        {
            glm::vec2 cam_area = GetCameraOrthoArea();
    
            float left   = position2D.x - (cam_area.x * 0.5f) / zoom;
            float bottom = position2D.y - (cam_area.y * 0.5f) / zoom;
    
            return glm::vec2(left, bottom);
        }
    
        glm::vec2 CameraManager::GetCameraOrthoArea() const
        {
            float baseAspectRatio = baseWindowSize.x / baseWindowSize.y;
    
            glm::vec2 screenSize = GameManager::GetInstance().getWindowSize();
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
    
/*
        glm::mat4 CameraManager::GetViewMatrix() const
        {
            return glm::lookAt(glm::vec3(position2D, 1.0f), glm::vec3(position2D, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
*/
    
        glm::mat4 CameraManager::GetProjectionMatrix() const
        {
            glm::vec2 cam_area = GetCameraOrthoArea();
    
            return glm::ortho(
                position2D.x - (cam_area.x * 0.5f) / zoom,
                position2D.x + (cam_area.x * 0.5f) / zoom,
                position2D.y + (cam_area.y * 0.5f) / zoom,
                position2D.y - (cam_area.y * 0.5f) / zoom,
                -1.0f, 1.0f
            );
        }

        void CameraManager::reset()
        {
            position2D = glm::vec2(0.0f);
        }
    
        void CameraManager::SetFocusObject(Object *Obj, float xoffset, float yoffset)
        {
            position2D.x = (Obj->GetPosition().x + Obj->GetSize().x / 2 + xoffset);
            position2D.y = (Obj->GetPosition().y + Obj->GetSize().y / 2 + yoffset);
        }
    
        void CameraManager::SetFocusPosition(glm::vec2 NewPosition)
        {
            position2D.x =  NewPosition.x;
            position2D.y =  NewPosition.y;
        }
    
        glm::vec2 CameraManager::GetFocusPosition() const
        {
            return glm::vec2(position2D.x , position2D.y );
        }

        void CameraManager::MoveFocus(const glm::vec2 &offset)
        {
            position2D += offset;
        }

        float CameraManager::GetZoom() const
        {
            return zoom;
        }

        void CameraManager::SetZoom(float newZoom)
        {
            zoom = glm::clamp(newZoom, 0.25f, 6.0f);
        }

        void CameraManager::AdjustZoom(float zoomOffset)
        {
            SetZoom(zoom + zoomOffset);
        }
    
        bool CameraManager::IsObjectInsideCameraArea(Object *Obj)
        {
            glm::vec2 cam_area = GetCameraOrthoArea();
    
            float left   = position2D.x - (cam_area.x * 0.5f) / zoom;
            float right  = position2D.x + (cam_area.x * 0.5f) / zoom;
            float top    = position2D.y + (cam_area.y * 0.5f) / zoom;
            float bottom = position2D.y - (cam_area.y * 0.5f) / zoom;
    
            glm::vec2 objPos = Obj->GetPosition();
            glm::vec2 objSize = Obj->GetSize();
    
            return (objPos.x + objSize.x > left &&
                    objPos.x < right &&
                    objPos.y + objSize.y > bottom &&
                    objPos.y < top);
        }

        void CameraManager::Classic2DProcessKeyboard(Camera_Movement direction, float deltaTime)
        {
            Classic2DCameraController::ProcessKeyboard(*this, direction, deltaTime);
        }

        void CameraManager::Classic2DProcessMouseMovement(float xoffset, float yoffset)
        {
            Classic2DCameraController::ProcessMouseMovement(*this, xoffset, yoffset);
        }

        void CameraManager::Classic2DProcessMouseScroll(float xoffset, float yoffset)
        {
            Classic2DCameraController::ProcessMouseScroll(*this, xoffset, yoffset);
        }

    #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

        void CameraManager::init(glm::vec2 baseSize)
        {
            this->baseWindowSize = baseSize;
            
            reset();

            updateCameraVectors();
            
            RUNTIME_INFO("Camera manager initialized");
        }

        
        void CameraManager::reset()
        {
            this->front = glm::vec3(0.0f, 0.0f, -1.0f);
            this->movementSpeed = 2.5f;
            this->mouseSensitivity = 0.1f;
            this->zoom = 45.0f;
            this->frontBody = glm::vec3(0.0f, 0.0f, -1.0f);
            this->position = glm::vec3(0.0f, 0.0f, 0.0f);
            this->worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            this->yaw = -90.0f;
            this->pitch = 0.0f;
            this->constrainPitch = true;

            this->baseAspectRatio = baseWindowSize.x / baseWindowSize.y;        
        }

        float CameraManager::GetMovementSpeed() const
        {
            return this->movementSpeed;
        }

        void CameraManager::SetMovementSpeed(float newMovementSpeed)
        {
            this->movementSpeed = newMovementSpeed;
        }

        float CameraManager::GetMouseSensitivity() const
        {
            return this->mouseSensitivity;
        }

        void CameraManager::SetMouseSensitivity(float newMouseSensitivity)
        {
            this->mouseSensitivity = newMouseSensitivity;
        }

        float CameraManager::GetZoom() const
        {
            return this->zoom;
        }

        void CameraManager::SetZoom(float newZoom)
        {
            this->zoom = glm::clamp(newZoom, 1.0f, 45.0f);
        }

        void CameraManager::AdjustZoom(float zoomOffset)
        {
            SetZoom(this->zoom + zoomOffset);
        }


        glm::vec3 CameraManager::GetPosition() const
        {
            return this->position;
        }
    
        glm::vec3 CameraManager::GetViewDirection() const
        {
            return this->front;
        }

        glm::vec3 CameraManager::GetBodyDirection() const
        {
            return this->frontBody;
        }

        glm::vec3 CameraManager::GetRightDirection() const
        {
            return this->right;
        }

        void CameraManager::Move(const glm::vec3 &offset)
        {
            position += offset;
        }

        void CameraManager::SetFocusObject(glm::vec3 from, Object *ObjToLook, float xoffset, float yoffset)
        {
            if (!ObjToLook)
            {
                return;
            }

            glm::vec3 target = ObjToLook->GetPosition() + ObjToLook->GetSize() * 0.5f + glm::vec3(xoffset, yoffset, 0.0f);
            SetFocusPosition(from, target);
        }

        void CameraManager::SetFocusPosition(glm::vec3 from, glm::vec3 lookingAt)
        {
            position = from;

            glm::vec3 direction = glm::normalize(lookingAt - from);
            yaw = glm::degrees(std::atan2(direction.z, direction.x));
            pitch = glm::degrees(std::asin(direction.y));

            if (constrainPitch)
            {
                pitch = glm::clamp(pitch, -89.0f, 89.0f);
            }

            updateCameraVectors();
        }

        void CameraManager::RotateBy(float yawOffset, float pitchOffset)
        {
            yaw += yawOffset;
            pitch += pitchOffset;

            if (constrainPitch)
            {
                pitch = glm::clamp(pitch, -89.0f, 89.0f);
            }

            updateCameraVectors();
        }

        glm::mat4 CameraManager::GetViewMatrix() const
        {
            return glm::lookAt(position, position + front, up);
        }
    
        glm::mat4 CameraManager::GetProjectionMatrix() const
        {
            return glm::perspective(glm::radians(zoom), GameManager::GetInstance().getWindowSize().x / GameManager::GetInstance().getWindowSize().y, 0.1f, 300.0f);
        }

        void CameraManager::Classic3DProcessKeyboard(Camera_Movement direction, float deltaTime)
        {
            Classic3DCameraController::ProcessKeyboard(*this, direction, deltaTime);
        }
    
        void CameraManager::Classic3DProcessMouseMovement(float xoffset, float yoffset)
        {
            Classic3DCameraController::ProcessMouseMovement(*this, InputManager::GetInstance(), xoffset, yoffset);
        }
    
        void CameraManager::Classic3DProcessMouseScroll(float xoffset, float yoffset)
        {
            Classic3DCameraController::ProcessMouseScroll(*this, InputManager::GetInstance(), xoffset, yoffset);
        }

        void CameraManager::ResetMouseLookReference()
        {
            InputManager::GetInstance().ResetMouseLookReference();
        }
    
        void CameraManager::updateCameraVectors()
        {
            glm::vec3 front_tmp;
            glm::vec3 frontbody_tmp;
    
            front_tmp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front_tmp.y = sin(glm::radians(pitch));
            front_tmp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    
            frontbody_tmp.x = cos(glm::radians(yaw));
            frontbody_tmp.y = 0.0f;
            frontbody_tmp.z = sin(glm::radians(yaw));
    
            frontBody = glm::normalize(frontbody_tmp);
            front = glm::normalize(front_tmp);
            right = glm::normalize(glm::cross(front, worldUp));
            up = glm::normalize(glm::cross(right, front));
        }

    #else
        #error "[CameraManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif









}
