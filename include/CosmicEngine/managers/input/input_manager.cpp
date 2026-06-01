/**
 * @file input_manager.cpp
 * @brief Implements the input manager used to poll keyboard, mouse, text, and controller state.
 */

#include "input_manager.hpp"

#include "../game_manager.hpp"
#include "../camera/camera_manager.hpp"
#include <cosmic_project_config.hpp>
#include "../../utils/log.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace
{
    using nlohmann::json;

    constexpr float kDefaultGamepadDeadzone = 0.15f;

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

    std::filesystem::path GetDefaultInputConfigPath()
    {
        std::filesystem::path configDirectory = std::filesystem::current_path() / "config";
        return configDirectory / (std::string(CosmicEngine::ProjectConfig::kProjectName) + "_input.json");
    }

    json BindingToJson(const CosmicEngine::InputActionBinding &binding)
    {
        return json{
            {"keyboard", binding.keyboardKeys},
            {"gamepadButtons", binding.gamepadButtons},
            {"gamepadAxesPositive", binding.gamepadAxesPositive},
            {"gamepadAxesNegative", binding.gamepadAxesNegative}
        };
    }

    CosmicEngine::InputActionBinding BindingFromJson(const json &bindingJson)
    {
        CosmicEngine::InputActionBinding binding;

        if (bindingJson.contains("keyboard") && bindingJson["keyboard"].is_array())
        {
            binding.keyboardKeys = bindingJson["keyboard"].get<std::vector<int>>();
        }

        if (bindingJson.contains("gamepadButtons") && bindingJson["gamepadButtons"].is_array())
        {
            binding.gamepadButtons = bindingJson["gamepadButtons"].get<std::vector<int>>();
        }

        if (bindingJson.contains("gamepadAxesPositive") && bindingJson["gamepadAxesPositive"].is_array())
        {
            binding.gamepadAxesPositive = bindingJson["gamepadAxesPositive"].get<std::vector<int>>();
        }

        if (bindingJson.contains("gamepadAxesNegative") && bindingJson["gamepadAxesNegative"].is_array())
        {
            binding.gamepadAxesNegative = bindingJson["gamepadAxesNegative"].get<std::vector<int>>();
        }

        return binding;
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
		RUNTIME_LIFECYCLE("Input manager", "created");
    }

    InputManager::~InputManager()
    {
		RUNTIME_LIFECYCLE("Input manager", "destroyed");
    }

    void InputManager::init(GLFWwindow *window)
    {
        configFilePath = GetDefaultInputConfigPath().string();
        gamepadDeadzone = kDefaultGamepadDeadzone;
        bindingsDirty = false;
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

        LoadBindingsFromJson();
        RegisterCoreActions();
        SaveBindingsToJson(false);
        ResetMouseLookReference();

        RUNTIME_LIFECYCLE("Input manager", "initialized");
    }

    
    void InputManager::shutdown()
    {
        SaveBindingsToJson(false);
        ResetMouseSettings();
		RUNTIME_LIFECYCLE("Input manager", "shutdown");
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
                        if (std::abs(axisValue) < gamepadDeadzone)
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
                    if (std::abs(axisValue) < gamepadDeadzone)
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

        UpdateActionStates();
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

    void InputManager::RegisterCoreActions()
    {
        RegisterAction("camera_move_forward", {{GLFW_KEY_W}, {GLFW_GAMEPAD_BUTTON_DPAD_UP}, {}, {GLFW_GAMEPAD_AXIS_LEFT_Y}});
        RegisterAction("camera_move_backward", {{GLFW_KEY_S}, {GLFW_GAMEPAD_BUTTON_DPAD_DOWN}, {GLFW_GAMEPAD_AXIS_LEFT_Y}, {}});
        RegisterAction("camera_move_left", {{GLFW_KEY_A}, {GLFW_GAMEPAD_BUTTON_DPAD_LEFT}, {}, {GLFW_GAMEPAD_AXIS_LEFT_X}});
        RegisterAction("camera_move_right", {{GLFW_KEY_D}, {GLFW_GAMEPAD_BUTTON_DPAD_RIGHT}, {GLFW_GAMEPAD_AXIS_LEFT_X}, {}});
        RegisterAction("camera_move_up", {{GLFW_KEY_SPACE}, {}, {GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER}, {}});
        RegisterAction("camera_move_down", {{GLFW_KEY_LEFT_SHIFT}, {}, {GLFW_GAMEPAD_AXIS_LEFT_TRIGGER}, {}});
        RegisterAction("camera_look_left", {{GLFW_KEY_LEFT}, {}, {}, {GLFW_GAMEPAD_AXIS_RIGHT_X}});
        RegisterAction("camera_look_right", {{GLFW_KEY_RIGHT}, {}, {GLFW_GAMEPAD_AXIS_RIGHT_X}, {}});
        RegisterAction("camera_look_up", {{GLFW_KEY_PAGE_UP}, {}, {}, {GLFW_GAMEPAD_AXIS_RIGHT_Y}});
        RegisterAction("camera_look_down", {{GLFW_KEY_PAGE_DOWN}, {}, {GLFW_GAMEPAD_AXIS_RIGHT_Y}, {}});

        RegisterAction("ui_nav_next", {{GLFW_KEY_TAB}, {}, {}, {}});
        RegisterAction("ui_nav_up", {{GLFW_KEY_UP}, {GLFW_GAMEPAD_BUTTON_DPAD_UP}, {}, {GLFW_GAMEPAD_AXIS_LEFT_Y}});
        RegisterAction("ui_nav_down", {{GLFW_KEY_DOWN}, {GLFW_GAMEPAD_BUTTON_DPAD_DOWN}, {GLFW_GAMEPAD_AXIS_LEFT_Y}, {}});
        RegisterAction("ui_nav_left", {{GLFW_KEY_LEFT}, {GLFW_GAMEPAD_BUTTON_DPAD_LEFT}, {}, {GLFW_GAMEPAD_AXIS_LEFT_X}});
        RegisterAction("ui_nav_right", {{GLFW_KEY_RIGHT}, {GLFW_GAMEPAD_BUTTON_DPAD_RIGHT}, {GLFW_GAMEPAD_AXIS_LEFT_X}, {}});
        RegisterAction("ui_submit", {{GLFW_KEY_ENTER, GLFW_KEY_SPACE}, {GLFW_GAMEPAD_BUTTON_A}, {}, {}});
        RegisterAction("ui_back", {{GLFW_KEY_ESCAPE}, {GLFW_GAMEPAD_BUTTON_B, GLFW_GAMEPAD_BUTTON_START}, {}, {}});

        RegisterAction("system_exit_game", {{GLFW_KEY_ESCAPE}, {GLFW_GAMEPAD_BUTTON_START}, {}, {}});
    }

    void InputManager::UpdateActionStates()
    {
        for (auto &[name, state] : actionDownState)
        {
            state = false;
        }

        for (auto &[name, state] : actionUpState)
        {
            state = false;
        }

        for (const auto &[name, binding] : actionBindings)
        {
            const float strength = ComputeActionStrength(binding);
            const bool isActive = strength > 0.0f;
            const bool wasActive = actionHeldState[name];

            actionStrengthState[name] = strength;
            actionHeldState[name] = isActive;
            actionDownState[name] = isActive && !wasActive;
            actionUpState[name] = !isActive && wasActive;
        }
    }

    float InputManager::ComputeActionStrength(const InputActionBinding &binding) const
    {
        float strength = 0.0f;

        for (int key : binding.keyboardKeys)
        {
            auto keyIt = keyHeldState.find(key);
            if (keyIt != keyHeldState.end() && keyIt->second)
            {
                strength = 1.0f;
            }
        }

        for (int button : binding.gamepadButtons)
        {
            auto buttonIt = joystickButtonState.find(button);
            if (buttonIt != joystickButtonState.end() && buttonIt->second)
            {
                strength = 1.0f;
            }
        }

        for (int axis : binding.gamepadAxesPositive)
        {
            strength = std::max(strength, std::max(0.0f, GetJoystickAxis(axis)));
        }

        for (int axis : binding.gamepadAxesNegative)
        {
            strength = std::max(strength, std::max(0.0f, -GetJoystickAxis(axis)));
        }

        return strength;
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

    int InputManager::GetFirstKeyJustPressed() const
    {
        for (const auto &[key, pressed] : keyDownState)
        {
            if (pressed) return key;
        }
        return -1;
    }

    int InputManager::GetFirstGamepadButtonJustPressed() const
    {
        for (const auto &[button, pressed] : joystickButtonDownState)
        {
            if (pressed) return button;
        }
        return -1;
    }

    float InputManager::GetGamepadDeadzone() const
    {
        return gamepadDeadzone;
    }

    void InputManager::SetGamepadDeadzone(float value)
    {
        gamepadDeadzone = std::clamp(value, 0.0f, 0.99f);
        bindingsDirty = true;
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

    void InputManager::RegisterAction(const std::string &actionName, const InputActionBinding &binding)
    {
        if (actionBindings.find(actionName) != actionBindings.end())
        {
            return;
        }

        actionBindings[actionName] = binding;
        actionHeldState[actionName] = false;
        actionDownState[actionName] = false;
        actionUpState[actionName] = false;
        actionStrengthState[actionName] = 0.0f;
        bindingsDirty = true;
    }

    void InputManager::SetActionBinding(const std::string &actionName, const InputActionBinding &binding)
    {
        actionBindings[actionName] = binding;
        actionHeldState[actionName] = false;
        actionDownState[actionName] = false;
        actionUpState[actionName] = false;
        actionStrengthState[actionName] = 0.0f;
        bindingsDirty = true;
    }

    bool InputManager::HasAction(const std::string &actionName) const
    {
        return actionBindings.find(actionName) != actionBindings.end();
    }

    InputActionBinding InputManager::GetActionBinding(const std::string &actionName) const
    {
        auto it = actionBindings.find(actionName);
        if (it == actionBindings.end())
        {
            return {};
        }

        return it->second;
    }

    bool InputManager::IsActionPressed(const std::string &actionName, KeyEventType eventType) const
    {
        switch (eventType)
        {
        case KeyEventType::KeyDown:
            return actionDownState.count(actionName) && actionDownState.at(actionName);
        case KeyEventType::KeyUp:
            return actionUpState.count(actionName) && actionUpState.at(actionName);
        case KeyEventType::KeyRelease:
            return actionHeldState.count(actionName) && actionHeldState.at(actionName);
        default:
            return false;
        }
    }

    float InputManager::GetActionStrength(const std::string &actionName) const
    {
        auto it = actionStrengthState.find(actionName);
        if (it == actionStrengthState.end())
        {
            return 0.0f;
        }

        return it->second;
    }

    float InputManager::GetActionAxis(const std::string &negativeActionName, const std::string &positiveActionName) const
    {
        return GetActionStrength(positiveActionName) - GetActionStrength(negativeActionName);
    }

    void InputManager::SetConfigFilePath(const std::string &path)
    {
        configFilePath = path;
    }

    std::string InputManager::GetConfigFilePath() const
    {
        return configFilePath;
    }

    bool InputManager::LoadBindingsFromJson()
    {
        if (configFilePath.empty())
        {
            return false;
        }

        std::ifstream input(configFilePath, std::ios::binary);
        if (!input.is_open())
        {
            return false;
        }

        try
        {
            json document = json::parse(input);
            if (document.contains("settings") && document["settings"].is_object())
            {
                gamepadDeadzone = std::clamp(document["settings"].value("gamepadDeadzone", kDefaultGamepadDeadzone), 0.0f, 0.99f);
            }

            if (document.contains("actions") && document["actions"].is_object())
            {
                for (auto it = document["actions"].begin(); it != document["actions"].end(); ++it)
                {
                    actionBindings[it.key()] = BindingFromJson(it.value());
                    actionHeldState[it.key()] = false;
                    actionDownState[it.key()] = false;
                    actionUpState[it.key()] = false;
                    actionStrengthState[it.key()] = 0.0f;
                }
            }

            bindingsDirty = false;
            return true;
        }
        catch (const std::exception &exception)
        {
            RUNTIME_WARNING("[InputManager] Failed to parse input config '" << configFilePath << "': " << exception.what());
            return false;
        }
    }

    bool InputManager::SaveBindingsToJson(bool force)
    {
        if (configFilePath.empty())
        {
            return false;
        }

        if (!bindingsDirty && !force)
        {
            return true;
        }

        json document;
        document["version"] = 1;
        document["settings"] = {
            {"gamepadDeadzone", gamepadDeadzone}
        };

        json actions = json::object();
        for (const auto &[name, binding] : actionBindings)
        {
            actions[name] = BindingToJson(binding);
        }
        document["actions"] = std::move(actions);

        try
        {
            const std::filesystem::path outputPath(configFilePath);
            std::filesystem::create_directories(outputPath.parent_path());

            std::ofstream output(configFilePath, std::ios::binary | std::ios::trunc);
            if (!output.is_open())
            {
                RUNTIME_WARNING("[InputManager] Failed to open input config for writing: " << configFilePath);
                return false;
            }

            output << document.dump(2);
            bindingsDirty = false;
            return true;
        }
        catch (const std::exception &exception)
        {
            RUNTIME_WARNING("[InputManager] Failed to save input config '" << configFilePath << "': " << exception.what());
            return false;
        }
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

        RUNTIME_INFO("[InputManager] Mouse settings reset.");
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