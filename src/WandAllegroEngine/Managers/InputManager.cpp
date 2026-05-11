#include "InputManager.hpp"
#include "GameManager.hpp"
#include "CameraManager.hpp"

namespace WandEngine
{

    InputManager::InputManager() : MouseSprite(nullptr), MouseSpriteOffSet(WAND_VEC2(0,0)), MouseSpriteSize(WAND_SIZE(32,32))
    {

    }

    InputManager::~InputManager()
    {
        ResetMouseSettings();

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

    WAND_VEC2 InputManager::GetMousePosition() const
    {
        int x, y;
        al_get_mouse_cursor_position(&x, &y);
        return WAND_VEC2(CameraManager::GetInstance().GetPosition().x + x, CameraManager::GetInstance().GetPosition().y + y);
    }

    void InputManager::ResetMouseSettings()
    {
        if(MouseSprite)
        {
            this->MouseSprite = nullptr;
        }
        ALLEGRO_DISPLAY *display = al_get_current_display();
        if (display)
        {
            al_show_mouse_cursor(display);
        }
        this->MouseSpriteOffSet = WAND_VEC2(0, 0);
        this->MouseSpriteSize = WAND_SIZE(32, 32);
    }


    void InputManager::SetMouseSprite(ALLEGRO_BITMAP* NewMouseSprite)
    {
        if(!MouseSprite)
        {
            ALLEGRO_DISPLAY *display = al_get_current_display();
            if (display)
            {
                al_hide_mouse_cursor(display);
            }
        }
        this->MouseSprite = NewMouseSprite;
    }

    void InputManager::SetMouseSpriteOffSet(WAND_VEC2 MouseSpriteOffSet)
    {
        this->MouseSpriteOffSet = MouseSpriteOffSet;
    }

    void InputManager::SetMouseSpriteSize(WAND_SIZE MouseSpriteSize)
    {
        this->MouseSpriteSize = MouseSpriteSize;
    }

    void InputManager::DrawMouseSprite()
    {
        if(MouseSprite)
        {
            al_draw_tinted_scaled_rotated_bitmap(MouseSprite, al_map_rgba(255,255,255, 255), 0, 0, GetMousePosition().x, GetMousePosition().y, MouseSpriteSize.width/al_get_bitmap_width(MouseSprite), MouseSpriteSize.height/al_get_bitmap_height(MouseSprite), 0, NULL );
        }
    }


}
