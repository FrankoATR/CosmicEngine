#include "Camera3D.hpp"
#include "../../../Managers/GameManager.hpp"

namespace WandEngine
{
    Camera3D::Camera3D(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5), MouseSensitivity(0.1f), Zoom(45.0f), FrontBody(glm::vec3(0.0f, 0.0f, -1.0f))
    {
        lastX = GameManager::GetInstance().GetWindowsSize().x / 2.0f;
        lastY = GameManager::GetInstance().GetWindowsSize().y / 2.0f;
        firstMouse = true;
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    Camera3D::Camera3D(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(2.5), MouseSensitivity(0.1f), Zoom(45.0f)
    {
        lastX = GameManager::GetInstance().GetWindowsSize().x / 2.0f;
        lastY = GameManager::GetInstance().GetWindowsSize().y / 2.0f;
        firstMouse = true;
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 Camera3D::GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    glm::mat4 Camera3D::GetProjectionMatrix(float aspectRatio)
    {
        return glm::perspective(glm::radians(Zoom), aspectRatio, 0.1f, 100.0f);
    }


    void Camera3D::ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += FrontBody * velocity;
        if (direction == BACKWARD)
            Position -= FrontBody * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position.y += velocity;
        if (direction == DOWN)
            Position.y -= velocity;
    }

    void Camera3D::ProcessMouseMovement(float xpos, float ypos, GLboolean constrainPitch)
    {

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

        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void Camera3D::ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

    void Camera3D::updateCameraVectors()
    {
        glm::vec3 front;
        glm::vec3 frontbody;

        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        frontbody.x = cos(glm::radians(Yaw));
        frontbody.y = 0.0f;
        frontbody.z = sin(glm::radians(Yaw));

        FrontBody = glm::normalize(frontbody);
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
}