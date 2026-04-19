/**
 * @file input_manager.cpp
 * @brief Implements the input manager used to poll keyboard, mouse, text, and controller state.
 */

#include "input_manager.hpp"

#include "../game_manager.hpp"
#include "../camera/camera_manager.hpp"
#include "../../utils/log.hpp"

#include <cmath>
#include <iostream>

namespace
{
    /**
     * @brief Converts the GLFW cursor position into normalized viewport coordinates.
     */
    bool GetNormalizedCursorInViewport(GLFWwindow *window, float &normalizedX, float &normalizedY)
    {
        if (!window)
        {
            normalizedX = 0.0f;
            normalizedY = 0.0f;
            return false;
        }

        double cursorX = 0.0;
        double cursorY = 0.0;
        glfwGetCursorPos(window, &cursorX, &cursorY);

        int windowWidth = 0;
        int windowHeight = 0;
        int framebufferWidth = 0;
        int framebufferHeight = 0;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        if (windowWidth <= 0 || windowHeight <= 0 || framebufferWidth <= 0 || framebufferHeight <= 0)
        {
            normalizedX = 0.0f;
            normalizedY = 0.0f;
            return false;
        }

        float cursorFramebufferX = static_cast<float>(cursorX) * (static_cast<float>(framebufferWidth) / static_cast<float>(windowWidth));
        float cursorFramebufferY = static_cast<float>(cursorY) * (static_cast<float>(framebufferHeight) / static_cast<float>(windowHeight));

        GLint viewport[4] = {0, 0, framebufferWidth, framebufferHeight};
        glGetIntegerv(GL_VIEWPORT, viewport);

        float viewportWidth = viewport[2] > 0 ? static_cast<float>(viewport[2]) : static_cast<float>(framebufferWidth);
        float viewportHeight = viewport[3] > 0 ? static_cast<float>(viewport[3]) : static_cast<float>(framebufferHeight);

        normalizedX = (cursorFramebufferX - static_cast<float>(viewport[0])) / viewportWidth;
        normalizedY = ((static_cast<float>(framebufferHeight) - cursorFramebufferY) - static_cast<float>(viewport[1])) / viewportHeight;
        return true;
    }
}

namespace CosmicEngine
{
    InputManager &InputManager::GetInstance()
    {
        static InputManager instance;
        return instance;
    }

    InputManager::InputManager()
    {
        RUNTIME_INFO("Input manager created");
    }

    InputManager::~InputManager()
    {
        RUNTIME_INFO("Input manager destroyed");
    }

    void InputManager::init(GLFWwindow *window)
    {
        MouseSprite = nullptr;
        MouseSpriteOffSet = glm::vec2(0.0f);
        MouseSpriteSize = glm::vec2(32.0f);
        cachedNormalizedMousePosition = glm::vec2(0.0f);
        hasCachedNormalizedMousePosition = false;
        activeMouseInput = true;
        lastMouseX = 0.0f;
        lastMouseY = 0.0f;
        firstMouseInput = true;
        disableMouse = false;
        activeGamepadId = -1;
        activeJoystickUsesGamepadApi = false;

        glfwSetCharCallback(window, [](GLFWwindow *, unsigned int codepoint)
        {
            InputManager::GetInstance().queuedCharacterInput.push_back(codepoint);
        });

        ResetMouseLookReference();

        RUNTIME_INFO("Input manager initialized");
    }

    
    void InputManager::shutdown()
    {
        ResetMouseSettings();
    }

    void InputManager::update(GLFWwindow *window)
    {
        for (auto &[key, state] : keyDownState)
            state = false;
        for (auto &[key, state] : keyUpState)
            state = false;
        for (auto &[button, state] : mouseButtonDownState)
            state = false;
        for (auto &[button, state] : mouseButtonUpState)
            state = false;
        for (auto &[button, state] : joystickButtonDownState)
            state = false;
        for (auto &[button, state] : joystickButtonUpState)
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

        activeGamepadId = -1;
        activeJoystickUsesGamepadApi = false;
        for (int joystickId = GLFW_JOYSTICK_1; joystickId <= GLFW_JOYSTICK_LAST; ++joystickId)
        {
            if (glfwJoystickPresent(joystickId) != GLFW_TRUE)
            {
                continue;
            }

            activeGamepadId = joystickId;
            activeJoystickUsesGamepadApi = glfwJoystickIsGamepad(joystickId) == GLFW_TRUE;
            if (activeJoystickUsesGamepadApi)
            {
                break;
            }

            if (activeGamepadId != -1)
            {
                break;
            }
        }

        if (activeGamepadId != -1)
        {
            if (activeJoystickUsesGamepadApi)
            {
                GLFWgamepadstate gamepadState;
                if (glfwGetGamepadState(activeGamepadId, &gamepadState) == GLFW_TRUE)
                {
                    for (int button = 0; button <= GLFW_GAMEPAD_BUTTON_LAST; ++button)
                    {
                        bool isPressed = gamepadState.buttons[button] == GLFW_PRESS;
                        if (isPressed)
                        {
                            if (!joystickButtonState[button])
                            {
                                joystickButtonDownState[button] = true;
                                joystickButtonState[button] = true;
                            }
                        }
                        else if (joystickButtonState[button])
                        {
                            joystickButtonUpState[button] = true;
                            joystickButtonState[button] = false;
                        }
                    }

                    for (int axis = 0; axis <= GLFW_GAMEPAD_AXIS_LAST; ++axis)
                    {
                        float axisValue = gamepadState.axes[axis];
                        if (std::abs(axisValue) < 0.15f)
                        {
                            axisValue = 0.0f;
                        }
                        joystickAxisState[axis] = axisValue;
                    }
                }
                else
                {
                    activeGamepadId = -1;
                    activeJoystickUsesGamepadApi = false;
                }
            }
            else
            {
                int axisCount = 0;
                const float *axes = glfwGetJoystickAxes(activeGamepadId, &axisCount);
                int buttonCount = 0;
                const unsigned char *buttons = glfwGetJoystickButtons(activeGamepadId, &buttonCount);

                for (auto &[axis, value] : joystickAxisState)
                {
                    value = 0.0f;
                }

                for (int button = 0; button <= GLFW_GAMEPAD_BUTTON_LAST; ++button)
                {
                    bool isPressed = buttons && button < buttonCount && buttons[button] == GLFW_PRESS;
                    if (isPressed)
                    {
                        if (!joystickButtonState[button])
                        {
                            joystickButtonDownState[button] = true;
                            joystickButtonState[button] = true;
                        }
                    }
                    else if (joystickButtonState[button])
                    {
                        joystickButtonUpState[button] = true;
                        joystickButtonState[button] = false;
                    }
                }

                auto setAxisFromRaw = [this, axes, axisCount](int mappedAxis, int rawAxis)
                {
                    float axisValue = (axes && rawAxis < axisCount) ? axes[rawAxis] : 0.0f;
                    if (std::abs(axisValue) < 0.15f)
                    {
                        axisValue = 0.0f;
                    }
                    joystickAxisState[mappedAxis] = axisValue;
                };

                setAxisFromRaw(GLFW_GAMEPAD_AXIS_LEFT_X, 0);
                setAxisFromRaw(GLFW_GAMEPAD_AXIS_LEFT_Y, 1);
                setAxisFromRaw(GLFW_GAMEPAD_AXIS_RIGHT_X, 2);
                setAxisFromRaw(GLFW_GAMEPAD_AXIS_RIGHT_Y, 3);
                setAxisFromRaw(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, 4);
                setAxisFromRaw(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, 5);
            }
        }

        if (activeGamepadId == -1)
        {
            for (auto &[button, state] : joystickButtonState)
                state = false;
            for (auto &[axis, value] : joystickAxisState)
                value = 0.0f;
        }

        UpdateNormalizedMousePosition(window);
    }

    void InputManager::UpdateNormalizedMousePosition(GLFWwindow *window)
    {
        float normalizedX = 0.0f;
        float normalizedY = 0.0f;

        hasCachedNormalizedMousePosition = GetNormalizedCursorInViewport(window, normalizedX, normalizedY);

        if (hasCachedNormalizedMousePosition)
        {
            cachedNormalizedMousePosition = glm::vec2(normalizedX, normalizedY);
        }
        else
        {
            cachedNormalizedMousePosition = glm::vec2(0.0f);
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

    bool InputManager::IsJoystickButtonPressed(int button, KeyEventType eventType) const
    {
        switch (eventType)
        {
        case KeyEventType::KeyDown:
            return joystickButtonDownState.count(button) && joystickButtonDownState.at(button);
        case KeyEventType::KeyUp:
            return joystickButtonUpState.count(button) && joystickButtonUpState.at(button);
        case KeyEventType::KeyRelease:
            return joystickButtonState.count(button) && joystickButtonState.at(button);
        default:
            return false;
        }
    }

    float InputManager::GetJoystickAxis(int axis) const
    {
        auto it = joystickAxisState.find(axis);
        if (it == joystickAxisState.end())
        {
            return 0.0f;
        }

        return it->second;
    }

    bool InputManager::HasGamepad() const
    {
        return activeGamepadId != -1;
    }

    bool InputManager::PollCharacterInput(unsigned int &character)
    {
        if (queuedCharacterInput.empty())
        {
            character = 0;
            return false;
        }

        character = queuedCharacterInput.front();
        queuedCharacterInput.pop_front();
        return true;
    }

    void InputManager::ClearCharacterInput()
    {
        queuedCharacterInput.clear();
    }


    glm::vec2 InputManager::GetMousePosition() const
    {
        float normalizedX = cachedNormalizedMousePosition.x;
        float normalizedY = cachedNormalizedMousePosition.y;

        if (!hasCachedNormalizedMousePosition &&
            !GetNormalizedCursorInViewport(GameManager::GetInstance().getWindowPtr(), normalizedX, normalizedY))
        {
            return glm::vec2(0.0f);
        }

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            float ndcX = normalizedX * 2.0f - 1.0f;
            float ndcY = normalizedY * 2.0f - 1.0f;

            glm::mat4 inverseProjection = glm::inverse(CameraManager::GetInstance().GetProjectionMatrix());
            glm::vec4 worldPosition = inverseProjection * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);

            if (worldPosition.w != 0.0f)
            {
                worldPosition /= worldPosition.w;
            }

            return glm::vec2(worldPosition.x, worldPosition.y);
        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            glm::vec2 CameraPos = {CameraManager::GetInstance().GetPosition().x, CameraManager::GetInstance().GetPosition().y};
            glm::vec2 baseWin = {CameraManager::GetInstance().GetBaseWindowSize().x, CameraManager::GetInstance().GetBaseWindowSize().y};
            return glm::vec2(CameraPos.x + normalizedX * baseWin.x, CameraPos.y + (1.0f - normalizedY) * baseWin.y);
        #else
            return glm::vec2(0.0f);
        #endif
    }

    glm::vec2 InputManager::GetMousePosition_UI() const
    {
        float normalizedX = cachedNormalizedMousePosition.x;
        float normalizedY = cachedNormalizedMousePosition.y;

        if (!hasCachedNormalizedMousePosition &&
            !GetNormalizedCursorInViewport(GameManager::GetInstance().getWindowPtr(), normalizedX, normalizedY))
        {
            return glm::vec2(0.0f);
        }

        glm::vec2 baseSize = CameraManager::GetInstance().GetBaseWindowSize();
        return glm::vec2(normalizedX * baseSize.x, (1.0f - normalizedY) * baseSize.y);
    }

    void InputManager::SetActiveMouseInput()
    {
        activeMouseInput = true;
    }

    void InputManager::SetInactiveMouseInput()
    {
        activeMouseInput = false;
    }

    bool InputManager::GetIsMouseInputActive() const
    {
        return activeMouseInput;
    }

    void InputManager::ResetMouseLookReference()
    {
        glm::vec2 windowSize = GameManager::GetInstance().getWindowSize();
        lastMouseX = windowSize.x * 0.5f;
        lastMouseY = windowSize.y * 0.5f;
        firstMouseInput = true;
    }

    bool InputManager::TryGetMouseLookDelta(float xpos, float ypos, float &xoffset, float &yoffset)
    {
        xoffset = 0.0f;
        yoffset = 0.0f;

        if (!activeMouseInput)
        {
            return false;
        }

        if (firstMouseInput)
        {
            lastMouseX = xpos;
            lastMouseY = ypos;
            firstMouseInput = false;
            return false;
        }

        xoffset = xpos - lastMouseX;
        yoffset = lastMouseY - ypos;

        lastMouseX = xpos;
        lastMouseY = ypos;
        return true;
    }

    void InputManager::SetDisableMouse(bool value)
    {
        disableMouse = value;
        if (disableMouse)
        {
            glfwSetInputMode(GameManager::GetInstance().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(GameManager::GetInstance().getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        glfwSetCursorPos(GameManager::GetInstance().getWindowPtr(), GameManager::GetInstance().getWindowSize().x / 2, GameManager::GetInstance().getWindowSize().y / 2);
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

        std::cout << "Input manager settings reseted" << std::endl;
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

}