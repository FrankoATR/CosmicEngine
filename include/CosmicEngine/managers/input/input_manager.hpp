#ifndef COSMIC_INPUTMANAGER_HPP
#define COSMIC_INPUTMANAGER_HPP

#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <deque>
#include <map>
#include <functional>

namespace CosmicEngine
{

    enum KeyEventType
    {
        KeyDown,
        KeyUp,
        KeyRelease
    };

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
        std::deque<unsigned int> queuedCharacterInput;
        int activeGamepadId;
        bool activeJoystickUsesGamepadApi;

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

    public:
        static InputManager &GetInstance();

        void init(GLFWwindow* window);
        void shutdown();
        void update(GLFWwindow* window);

        bool IsKeyPressed(int keycode, KeyEventType eventType) const;
        bool IsMouseButtonPressed(int button, KeyEventType eventType) const;
        bool IsJoystickButtonPressed(int button) const;
        bool IsJoystickButtonPressed(int button, KeyEventType eventType) const;
        float GetJoystickAxis(int axis) const;
        bool HasGamepad() const;
        bool PollCharacterInput(unsigned int &character);
        void ClearCharacterInput();

        glm::vec2 GetMousePosition() const;
        glm::vec2 GetMousePosition_UI() const;

        void SetActiveMouseInput();
        void SetInactiveMouseInput();
        bool GetIsMouseInputActive() const;
        void ResetMouseLookReference();
        bool TryGetMouseLookDelta(float xpos, float ypos, float &xoffset, float &yoffset);

        void SetDisableMouse(bool value);
        bool GetIsDisableMouse();

        void ResetMouseSettings();
        void SetMouseSprite(unsigned int *NewMouseSprite);
        void SetMouseSpriteOffSet(glm::vec2 MouseSpriteOffSet);
        void SetMouseSpriteSize(glm::vec2 MouseSpriteSize);
        void DrawMouseSprite();
    };
}

#endif // COSMIC_INPUTMANAGER_HPP
