#ifndef COSMIC_INPUTMANAGER_HPP
#define COSMIC_INPUTMANAGER_HPP

/**
 * @file input_manager.hpp
 * @brief Declares the input manager used to query keyboard, mouse, text, and controller state.
 */

#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <deque>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace CosmicEngine
{
    /**
     * @brief Represents the transition mode used when querying an input.
     */
    enum KeyEventType
    {
        /** @brief The input was pressed during the current frame. */
        KeyDown,
        /** @brief The input was released during the current frame. */
        KeyUp,
        /** @brief The input is currently held down. */
        KeyRelease
    };

    /**
     * @brief Describes the configurable inputs that can trigger a named action.
     */
    struct InputActionBinding
    {
        std::vector<int> keyboardKeys;
        std::vector<int> gamepadButtons;
        std::vector<int> gamepadAxesPositive;
        std::vector<int> gamepadAxesNegative;
    };

    /**
     * @brief Centralizes polling and state queries for keyboard, mouse, and joystick devices.
     *
     * InputManager wraps GLFW input polling and splits input state into three
     * transition modes: KeyDown (just pressed this frame), KeyUp (just released),
     * and KeyRelease (currently held).  It also tracks mouse position, joystick
     * axes, and queued text input for text fields.
     *
     * @par Example — keyboard and mouse queries in a scene update
     * @code
     * // Exit on Escape:
     * if (INP_MN.IsKeyPressed(GLFW_KEY_ESCAPE, CosmicEngine::KeyDown))
     *     GM_MN.endprogram();
     *
     * // Spawn on right-click (only when UI is not hovered):
     * if (INP_MN.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT, CosmicEngine::KeyDown)
     *     && !UI_MN.IsMouseHoverAny())
     * {
     *     glm::vec2 mousePos = INP_MN.GetMousePosition();
     *     SpawnObjectAt(mousePos);
     * }
     * @endcode
     *
     * @par Example — gamepad / joystick input
     * @code
     * float axisX = INP_MN.GetJoystickAxis(GLFW_GAMEPAD_AXIS_LEFT_X);
     * float axisY = INP_MN.GetJoystickAxis(GLFW_GAMEPAD_AXIS_LEFT_Y);
     * bool jump   = INP_MN.IsJoystickButtonPressed(GLFW_GAMEPAD_BUTTON_A,
     *                                               CosmicEngine::KeyDown);
     * @endcode
     */
    class InputManager
    {
    private:
        InputManager();
        ~InputManager();
        InputManager(const InputManager &) = delete;
        InputManager &operator=(const InputManager &) = delete;

        std::map<int, bool> keyDownState;
        std::map<int, bool> keyUpState;
        std::map<int, bool> keyHeldState;
        std::map<int, bool> mouseButtonDownState;
        std::map<int, bool> mouseButtonUpState;
        std::map<int, bool> mouseButtonHeldState;
        std::map<int, bool> joystickButtonDownState;
        std::map<int, bool> joystickButtonUpState;
        std::map<int, bool> joystickButtonState;
        std::map<int, float> joystickAxisState;
        std::map<std::string, InputActionBinding> actionBindings;
        std::map<std::string, bool> actionDownState;
        std::map<std::string, bool> actionUpState;
        std::map<std::string, bool> actionHeldState;
        std::map<std::string, float> actionStrengthState;
        std::deque<unsigned int> queuedCharacterInput;
        int activeGamepadId;
        bool activeJoystickUsesGamepadApi;
        std::string configFilePath;
        float gamepadDeadzone;
        bool bindingsDirty;

        unsigned int *MouseSprite;
        glm::vec2 MouseSpriteOffSet;
        glm::vec2 MouseSpriteSize;
        glm::vec2 cachedNormalizedMousePosition;
        bool hasCachedNormalizedMousePosition;
        bool activeMouseInput;
        float lastMouseX;
        float lastMouseY;
        bool firstMouseInput;

        bool disableMouse;

        void UpdateNormalizedMousePosition(GLFWwindow* window);
        void RegisterCoreActions();
        void UpdateActionStates();
        float ComputeActionStrength(const InputActionBinding &binding) const;

    public:
        /** @brief Returns the singleton instance of the input manager. */
        static InputManager &GetInstance();

        /**
         * @brief Initializes the input subsystem and installs the required window callbacks.
         * @param window GLFW window associated with the runtime.
         */
        void init(GLFWwindow* window);
        /** @brief Releases transient input resources and clears cached state. */
        void shutdown();
        /**
         * @brief Polls current input state from GLFW.
         * @param window GLFW window associated with the runtime.
         */
        void update(GLFWwindow* window);

        /**
         * @brief Queries a keyboard key using the requested transition mode.
         * @param keycode GLFW key code.
         * @param eventType Transition mode to evaluate.
         * @return True when the key matches the requested input state.
         */
        bool IsKeyPressed(int keycode, KeyEventType eventType) const;
        /**
         * @brief Queries a mouse button using the requested transition mode.
         * @param button GLFW mouse button identifier.
         * @param eventType Transition mode to evaluate.
         * @return True when the mouse button matches the requested input state.
         */
        bool IsMouseButtonPressed(int button, KeyEventType eventType) const;
        /**
         * @brief Returns whether a joystick button is currently held.
         * @param button Joystick or gamepad button identifier.
         * @return True when the button is pressed.
         */
        bool IsJoystickButtonPressed(int button) const;
        /**
         * @brief Queries a joystick button using the requested transition mode.
         * @param button Joystick or gamepad button identifier.
         * @param eventType Transition mode to evaluate.
         * @return True when the joystick button matches the requested input state.
         */
        bool IsJoystickButtonPressed(int button, KeyEventType eventType) const;
        /** @brief Returns the GLFW keycode pressed this frame, or -1 when none was detected. */
        int GetFirstKeyJustPressed() const;
        /** @brief Returns the gamepad button pressed this frame, or -1 when none was detected. */
        int GetFirstGamepadButtonJustPressed() const;
        /**
         * @brief Returns the current normalized value of an analog joystick axis.
         * @param axis GLFW gamepad axis identifier.
         * @return Current axis value.
         */
        float GetJoystickAxis(int axis) const;
        /** @brief Returns the configured gamepad deadzone used by action bindings. */
        float GetGamepadDeadzone() const;
        /** @brief Updates the configured gamepad deadzone and marks the config dirty. */
        void SetGamepadDeadzone(float value);
        /**
         * @brief Returns whether an active controller has been detected.
         * @return True when a gamepad or joystick is available.
         */
        bool HasGamepad() const;
        /**
         * @brief Retrieves the next queued text input codepoint.
         * @param character Output codepoint when one is available.
         * @return True when a character was dequeued.
         */
        bool PollCharacterInput(unsigned int &character);
        /** @brief Clears all queued text input codepoints. */
        void ClearCharacterInput();

        /** @brief Registers a default binding for an action without overwriting an existing persisted binding. */
        void RegisterAction(const std::string &actionName, const InputActionBinding &binding);
        /** @brief Replaces the current binding for an action and marks the config dirty. */
        void SetActionBinding(const std::string &actionName, const InputActionBinding &binding);
        /** @brief Returns whether an action exists in the current binding table. */
        bool HasAction(const std::string &actionName) const;
        /** @brief Returns the current binding for an action, or an empty binding when it does not exist. */
        InputActionBinding GetActionBinding(const std::string &actionName) const;
        /** @brief Queries a named action using the requested transition mode. */
        bool IsActionPressed(const std::string &actionName, KeyEventType eventType) const;
        /** @brief Returns the current analog strength of the named action. */
        float GetActionStrength(const std::string &actionName) const;
        /** @brief Combines two actions into a signed axis where positive minus negative is returned. */
        float GetActionAxis(const std::string &negativeActionName, const std::string &positiveActionName) const;
        /** @brief Overrides the JSON file path used to persist input bindings. */
        void SetConfigFilePath(const std::string &path);
        /** @brief Returns the JSON file path used to persist input bindings. */
        std::string GetConfigFilePath() const;
        /** @brief Loads bindings from the configured JSON file when possible. */
        bool LoadBindingsFromJson();
        /** @brief Saves bindings to the configured JSON file when possible. */
        bool SaveBindingsToJson(bool force = false);

        /**
         * @brief Returns the mouse position in world-space coordinates.
         * @return Mouse position converted to the active scene space.
         */
        glm::vec2 GetMousePosition() const;
        /**
         * @brief Returns the mouse position in UI-space coordinates.
         * @return Mouse position converted to the base UI space.
         */
        glm::vec2 GetMousePosition_UI() const;

        /** @brief Enables mouse-look style processing. */
        void SetActiveMouseInput();
        /** @brief Disables mouse-look style processing. */
        void SetInactiveMouseInput();
        /**
         * @brief Returns whether mouse-look style processing is enabled.
         * @return True when active mouse input is enabled.
         */
        bool GetIsMouseInputActive() const;
        /** @brief Resets the reference used to compute relative mouse movement. */
        void ResetMouseLookReference();
        /**
         * @brief Computes a mouse delta from the previously stored mouse-look reference.
         * @param xpos Current cursor x position.
         * @param ypos Current cursor y position.
         * @param xoffset Output horizontal delta.
         * @param yoffset Output vertical delta.
         * @return True when a valid delta was produced.
         */
        bool TryGetMouseLookDelta(float xpos, float ypos, float &xoffset, float &yoffset);

        /**
         * @brief Enables or disables captured cursor mode.
         * @param value True to disable the system cursor.
         */
        void SetDisableMouse(bool value);
        /**
         * @brief Returns whether cursor capture mode is active.
         * @return True when the system cursor is disabled.
         */
        bool GetIsDisableMouse();

        /** @brief Restores the custom mouse sprite configuration to its defaults. */
        void ResetMouseSettings();
        /**
         * @brief Sets the texture identifier used for the custom mouse sprite.
         * @param NewMouseSprite Pointer to the sprite texture identifier.
         */
        void SetMouseSprite(unsigned int *NewMouseSprite);
        /**
         * @brief Sets the draw offset of the custom mouse sprite.
         * @param MouseSpriteOffSet Sprite offset in UI space.
         */
        void SetMouseSpriteOffSet(glm::vec2 MouseSpriteOffSet);
        /**
         * @brief Sets the draw size of the custom mouse sprite.
         * @param MouseSpriteSize Sprite size in UI space.
         */
        void SetMouseSpriteSize(glm::vec2 MouseSpriteSize);
        /** @brief Draws the configured custom mouse sprite. */
        void DrawMouseSprite();
    };
}

#endif // COSMIC_INPUTMANAGER_HPP
