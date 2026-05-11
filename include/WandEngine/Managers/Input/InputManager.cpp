#include "InputManager.hpp"

#include "../GameManager.hpp"
#include "../Camera/CameraManager.hpp"

#include <iostream>

namespace WandEngine
{

    InputManager::InputManager() : MouseSprite(nullptr), MouseSpriteOffSet(glm::vec2(0, 0)), MouseSpriteSize(glm::vec2(32, 32)), disableMouse(false)
    {
    }

    void InputManager::Init(GLFWwindow *window)
    {
        glfwSetCursorPosCallback(window, MouseCallback);
        glfwSetScrollCallback(window, ScrollCallback);
    }

    void InputManager::Update(GLFWwindow *window)
    {
        for (auto &[key, state] : keyDownState)
            state = false;
        for (auto &[key, state] : keyUpState)
            state = false;
        for (auto &[button, state] : mouseButtonDownState)
            state = false;
        for (auto &[button, state] : mouseButtonUpState)
            state = false;

        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
        {
            if (glfwGetKey(window, key) == GLFW_PRESS)
            {
                if (!keyHeldState[key])
                {
                    keyDownState[key] = true;
                    keyHeldState[key] = true;
                }
            }
            else if (keyHeldState[key])
            {
                keyUpState[key] = true;
                keyHeldState[key] = false;
            }
        }

        for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; ++button)
        {
            if (glfwGetMouseButton(window, button) == GLFW_PRESS)
            {
                if (!mouseButtonHeldState[button])
                {
                    mouseButtonDownState[button] = true;
                    mouseButtonHeldState[button] = true;
                }
            }
            else if (mouseButtonHeldState[button])
            {
                mouseButtonUpState[button] = true;
                mouseButtonHeldState[button] = false;
            }
        }
    }

    bool InputManager::IsKeyPressed(int keycode, KeyEventType eventType) const
    {
        switch (eventType)
        {
        case KeyEventType::KeyDown:
            return keyDownState.count(keycode) && keyDownState.at(keycode);
        case KeyEventType::KeyUp:
            return keyUpState.count(keycode) && keyUpState.at(keycode);
        case KeyEventType::KeyRelease:
            return keyHeldState.count(keycode) && keyHeldState.at(keycode);
        default:
            return false;
        }
    }

    bool InputManager::IsMouseButtonPressed(int button, KeyEventType eventType) const
    {
        switch (eventType)
        {
        case KeyEventType::KeyDown:
            return mouseButtonDownState.count(button) && mouseButtonDownState.at(button);
        case KeyEventType::KeyUp:
            return mouseButtonUpState.count(button) && mouseButtonUpState.at(button);
        case KeyEventType::KeyRelease:
            return mouseButtonHeldState.count(button) && mouseButtonHeldState.at(button);
        default:
            return false;
        }
    }

    bool InputManager::IsJoystickButtonPressed(int button) const
    {
        return joystickButtonState.count(button) && joystickButtonState.at(button);
    }

    void InputManager::MouseCallback(GLFWwindow *window, double xpos, double ypos)
    {
        CameraManager::GetInstance().ProcessMouseMovement(xpos, ypos);
    }

    void InputManager::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        CameraManager::GetInstance().ProcessMouseScroll(xoffset, yoffset);
    }

    glm::vec2 InputManager::GetMousePosition() const
    {
        double x, y;
        glfwGetCursorPos(GameManager::GetInstance().GetWindow(), &x, &y);
        float xpos = static_cast<float>(x);
        float ypos = static_cast<float>(y);

        glm::vec2 CameraPos = {CameraManager::GetInstance().GetPosition().x, CameraManager::GetInstance().GetPosition().y};
        glm::vec2 winSize = {GameManager::GetInstance().GetWindowsSize().x, GameManager::GetInstance().GetWindowsSize().y};
        glm::vec2 baseWin = {CameraManager::GetInstance().GetBaseWindowSize().x, CameraManager::GetInstance().GetBaseWindowSize().y};

        return glm::vec2(CameraPos.x + xpos * (baseWin.x/ winSize.x), CameraPos.y + ypos * (baseWin.y / winSize.y));
    }

    glm::vec2 InputManager::GetAbsoluteMousePosition() const
    {
        double x, y;
        glfwGetCursorPos(GameManager::GetInstance().GetWindow(), &x, &y);
        float xpos = static_cast<float>(x);
        float ypos = static_cast<float>(y);

        return glm::vec2(x, y);
    }

    void InputManager::SetDisableMouse(bool value)
    {
        disableMouse = value;
        if (disableMouse)
        {
            glfwSetInputMode(GameManager::GetInstance().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(GameManager::GetInstance().GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        glfwSetCursorPos(GameManager::GetInstance().GetWindow(), GameManager::GetInstance().GetWindowsSize().x / 2, GameManager::GetInstance().GetWindowsSize().y / 2);
    }

    bool InputManager::GetIsDisableMouse()
    {
        return disableMouse;
    }

    void InputManager::ResetMouseSettings()
    {
        if (MouseSprite)
        {
            this->MouseSprite = nullptr;
        }

        this->MouseSpriteOffSet = glm::vec2(0, 0);
        this->MouseSpriteSize = glm::vec2(32, 32);

#ifndef NDEBUG
        std::cout << "Input manager settings reseted" << std::endl;
#endif
    }

    void InputManager::SetMouseSprite(unsigned int *NewMouseSprite)
    {
        if (!MouseSprite)
        {
        }
        this->MouseSprite = NewMouseSprite;
    }

    void InputManager::SetMouseSpriteOffSet(glm::vec2 MouseSpriteOffSet)
    {
        this->MouseSpriteOffSet = MouseSpriteOffSet;
    }

    void InputManager::SetMouseSpriteSize(glm::vec2 MouseSpriteSize)
    {
        this->MouseSpriteSize = MouseSpriteSize;
    }

    void InputManager::DrawMouseSprite()
    {
        if (MouseSprite)
        {
        }
    }

    void InputManager::Clear()
    {
        ResetMouseSettings();

#ifndef NDEBUG
        std::cout << "Input manager cleared" << std::endl;
#endif
    }

    InputManager::~InputManager()
    {
#ifndef NDEBUG
        std::cout << "Input manager destroyed" << std::endl;
#endif
    }

}