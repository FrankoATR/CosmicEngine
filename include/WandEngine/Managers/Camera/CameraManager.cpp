#include "CameraManager.hpp"

#include "../GameManager.hpp"
#include "../../Models/Object/Object.hpp"
#include "../Body/BodyManager.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include "../../Utils/Log.hpp"


namespace WandEngine
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
        activeMouseInput = true;
    }

    void CameraManager::SetInactiveMouseInput() //TODO: on inputManager
    {
        activeMouseInput = false;
    }

    bool CameraManager::GetIsMouseInputActive() //TODO: on inputManager
    {
        return activeMouseInput;
    }

    glm::mat4 CameraManager::Get_UI_ProjectionMatrix() const
    {
        return glm::ortho(
            0.0f, baseWindowSize.x,
            baseWindowSize.y, 0.0f,
            -1.0f, 1.0f
        );
    }

    #if defined(GAME_2D_CONFIGURATION)

        void CameraManager::Init(glm::vec2 baseSize)
        {
            this->activeMouseInput = true;
            this->position2D = glm::vec2(0.0f);
            this->baseWindowSize = baseSize;
            this->zoom = 1.0f;

            Reset();

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
            float baseAspectRatio = baseAspectRatio;
    
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
    
        glm::mat4 CameraManager::GetViewMatrix() const
        {
            return glm::lookAt(glm::vec3(position2D, 1.0f), glm::vec3(position2D, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
    
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

        void CameraManager::Reset()
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

    #elif defined(GAME_3D_CONFIGURATION)

        void CameraManager::Init(glm::vec2 baseSize)
        {
            
            Reset();

            updateCameraVectors();
            
            RUNTIME_INFO("Camera manager initialized");
        }

        
        void CameraManager::Reset()
        {
            this->front = glm::vec3(0.0f, 0.0f, -1.0f);
            this->movementSpeed = 2.5f;
            this->mouseSensitivity = 0.1f;
            this->zoom = 45.0f;
            this->frontBody = glm::vec3(0.0f, 0.0f, -1.0f);
            this->activeMouseInput = true;
            this->lastX = GameManager::GetInstance().GetWindowsSize().x / 2.0f;
            this->lastY = GameManager::GetInstance().GetWindowsSize().y / 2.0f;
            this->firstMouse = true;
            this->position = glm::vec3(0.0f, 0.0f, 0.0f);
            this->worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
            this->yaw = -90.0f;
            this->pitch = 0.0f;
            this->firstMouse = true;
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


        glm::vec3 CameraManager::GetPosition() const
        {
            return this->position;
        }
    
        glm::vec3 CameraManager::GetViewDirection() const
        {
            return this->front;
        }

        glm::mat4 CameraManager::GetViewMatrix() const
        {
            return glm::lookAt(position, position + front, up);
        }
    
        glm::mat4 CameraManager::GetProjectionMatrix() const
        {
            return glm::perspective(glm::radians(zoom), GameManager::GetInstance().GetWindowsSize().x / GameManager::GetInstance().GetWindowsSize().y, 0.1f, 100.0f);
        }

        void CameraManager::Classic3DProcessKeyboard(Camera_Movement direction, float deltaTime)
        {
            float velocity = movementSpeed * deltaTime;
            if (direction == FORWARD)
                position += frontBody * velocity;
            if (direction == BACKWARD)
                position -= frontBody * velocity;
            if (direction == LEFT)
                position -= right * velocity;
            if (direction == RIGHT)
                position += right * velocity;
            if (direction == UP)
                position.y += velocity;
            if (direction == DOWN)
                position.y -= velocity;
        }
    
        void CameraManager::Classic3DProcessMouseMovement(float xoffset, float yoffset)
        {
            if(activeMouseInput)
            {
                float xpos = static_cast<float>(xoffset);
                float ypos = static_cast<float>(yoffset);

                if (firstMouse)
                {
                    lastX = xpos;
                    lastY = ypos;
                    firstMouse = false;
                }
        
                float xoffset = xpos - lastX;
                float yoffset = lastY - ypos;
        
                lastX = xpos;
                lastY = ypos;
        
                xoffset *= mouseSensitivity;
                yoffset *= mouseSensitivity;
        
                yaw += xoffset;
                pitch += yoffset;
        
                if (constrainPitch)
                {
                    if (pitch > 89.0f)
                        pitch = 89.0f;
                    if (pitch < -89.0f)
                        pitch = -89.0f;
                }
        
                updateCameraVectors();
            }

        }
    
        void CameraManager::Classic3DProcessMouseScroll(float xoffset, float yoffset)
        {
            if(activeMouseInput)
            {
                float xpos = static_cast<float>(xoffset);
                float ypos = static_cast<float>(yoffset);

                zoom -= (float)ypos;
                if (zoom < 1.0f)
                    zoom = 1.0f;
                if (zoom > 45.0f)
                    zoom = 45.0f;
            }
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
