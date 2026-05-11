#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#define GLFW_INCLUDE_NONE 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>
#include <functional>

namespace WandEngine
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
        std::map<int, bool> joystickButtonState;

        unsigned int *MouseSprite;
        glm::vec2 MouseSpriteOffSet;
        glm::vec2 MouseSpriteSize;

        bool disableMouse;

        static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
        static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        std::function<void(double, double)> mousePositionCallback;
        std::function<void(double, double)> mouseScrollCallback;

    public:
        static InputManager &GetInstance();

        void Init(GLFWwindow* window);
        void Shutdown();
        void Update(GLFWwindow* window);

        bool IsKeyPressed(int keycode, KeyEventType eventType) const;
        bool IsMouseButtonPressed(int button, KeyEventType eventType) const;
        bool IsJoystickButtonPressed(int button) const;

        glm::vec2 GetMousePosition() const;
        glm::vec2 GetMousePosition_UI() const;

        void SetDisableMouse(bool value);
        bool GetIsDisableMouse();

        void ResetMouseSettings();
        void SetMouseSprite(unsigned int *NewMouseSprite);
        void SetMouseSpriteOffSet(glm::vec2 MouseSpriteOffSet);
        void SetMouseSpriteSize(glm::vec2 MouseSpriteSize);
        void DrawMouseSprite();

        void SetMousePosition_Callback(std::function<void(double xpos, double ypos)>);
        void SetMouseScroll_Callback(std::function<void(double xoffset, double yoffset)>);
    };
}

#endif // INPUTMANAGER_HPP
