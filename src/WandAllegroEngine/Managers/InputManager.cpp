#include "InputManager.hpp"
#include "GameManager.hpp"
#include <allegro5/allegro.h>

namespace WandEngine
{

    InputManager::InputManager()
    {
    }

    InputManager::~InputManager()
    {
        al_unregister_event_source(GameManager::GetInstance().event_queue, al_get_mouse_event_source());
        al_unregister_event_source(GameManager::GetInstance().event_queue, al_get_keyboard_event_source());
        al_unregister_event_source(GameManager::GetInstance().event_queue, al_get_joystick_event_source());

        al_uninstall_keyboard();
        al_uninstall_mouse();
        al_uninstall_joystick();

        std::cout << "Input manager destroyed" << std::endl;
    }

    void InputManager::Init()
    {
        if (!al_install_keyboard())
            std::cerr << "Error to install keyboard" << std::endl;

        if (!al_install_mouse())
            std::cerr << "Error to install mouse" << std::endl;

        if (!al_install_joystick())
            std::cerr << "Error to install joystick." << std::endl;

        al_register_event_source(GameManager::GetInstance().event_queue, al_get_mouse_event_source());
        al_register_event_source(GameManager::GetInstance().event_queue, al_get_keyboard_event_source());
        al_register_event_source(GameManager::GetInstance().event_queue, al_get_joystick_event_source());
    }

    void InputManager::Update()
    {
        for (auto &[key, state] : keyDownState)
            state = false;
        for (auto &[key, state] : keyUpState)
            state = false;
        for (auto &[button, state] : mouseButtonDownState)
            state = false;
        for (auto &[button, state] : mouseButtonUpState)
            state = false;

    }

    void InputManager::ProcessEvent(const ALLEGRO_EVENT &event)
    {
        switch (event.type)
        {
        case ALLEGRO_EVENT_KEY_DOWN:
            keyDownState[event.keyboard.keycode] = true;
            keyHeldState[event.keyboard.keycode] = true;
            break;

        case ALLEGRO_EVENT_KEY_UP:
            keyUpState[event.keyboard.keycode] = true;
            keyHeldState[event.keyboard.keycode] = false;
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            mouseButtonDownState[event.mouse.button] = true;
            mouseButtonHeldState[event.mouse.button] = true;
            break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            mouseButtonUpState[event.mouse.button] = true;
            mouseButtonHeldState[event.mouse.button] = false;
            break;

        case ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN:
            joystickButtonState[event.joystick.button] = true;
            break;

        case ALLEGRO_EVENT_JOYSTICK_BUTTON_UP:
            joystickButtonState[event.joystick.button] = false;
            break;

        default:
            break;
        }
    }

    bool InputManager::IsKeyPressed(int keycode, KeyEventType eventType) const
    {
        switch (eventType)
        {
        case KeyDown:
            return keyDownState.count(keycode) && keyDownState.at(keycode);
        case KeyUp:
            return keyUpState.count(keycode) && keyUpState.at(keycode);
        case KeyRelease:
            return keyHeldState.count(keycode) && keyHeldState.at(keycode);
        default:
            return false;
        }
    }

    bool InputManager::IsMouseButtonPressed(int button, KeyEventType eventType) const
    {
        switch (eventType)
        {
        case KeyDown:
            return mouseButtonDownState.count(button) && mouseButtonDownState.at(button);
        case KeyUp:
            return mouseButtonUpState.count(button) && mouseButtonUpState.at(button);
        case KeyRelease:
            return mouseButtonHeldState.count(button) && mouseButtonHeldState.at(button);
        default:
            return false;
        }
    }

    bool InputManager::IsJoystickButtonPressed(int button) const
    {
        return joystickButtonState.count(button) && joystickButtonState.at(button);
    }
}
