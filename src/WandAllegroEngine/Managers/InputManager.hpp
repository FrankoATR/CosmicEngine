#ifndef INPUTMANAGER_HPP
#define INPUTMANAGER_HPP

#include <allegro5/allegro5.h>
#include <map>
#include "../Interfaces/Definitions.hpp"

namespace WandEngine
{

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

        ALLEGRO_BITMAP *MouseSprite;
        WAND_VEC2 MouseSpriteOffSet;
        WAND_SIZE MouseSpriteSize;

    public:
        static InputManager &GetInstance()
        {
            static InputManager instance;
            return instance;
        }

        void Init();
        void Update();
        void ProcessEvent(const ALLEGRO_EVENT &event);

        bool IsKeyPressed(int keycode, KeyEventType eventType) const;
        bool IsMouseButtonPressed(int button, KeyEventType eventType) const;
        bool IsJoystickButtonPressed(int button) const;

        WAND_VEC2 GetMousePosition() const;

        void ResetMouseSettings();
        void SetMouseSprite(ALLEGRO_BITMAP *NewMouseSprite);
        void SetMouseSpriteOffSet(WAND_VEC2 MouseSpriteOffSet);
        void SetMouseSpriteSize(WAND_SIZE MouseSpriteSize);
        void DrawMouseSprite();
    };
}

#endif // INPUTMANAGER_HPP
