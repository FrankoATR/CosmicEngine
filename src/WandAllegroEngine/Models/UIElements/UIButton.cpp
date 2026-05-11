#include "UIButton.hpp"
#include "../../Managers/InputManager.hpp"
#include "../../Managers/CameraManager.hpp"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

namespace WandEngine
{

    UIButton::UIButton(ALLEGRO_BITMAP *Sprite, const std::string &text, ALLEGRO_FONT *font, WAND_VEC2 Position, WAND_SIZE Size, bool visible, UIElement *parent) : 
        Sprite(Sprite), text(text), font(font), UIElement(Position, Size, visible, parent, UIElementType::Button), textColor(WAND_COLOR(255, 255, 255)), isPressed(false)
    {
    }

    UIButton::~UIButton()
    {
    }

    void UIButton::Update(float deltaTime)
    {
        HandleInput();

        UIElement::Update(deltaTime);
    }

    void UIButton::Draw()
    {
        if (!visible)
        {
            return;
        }



        if (Sprite)
        {
            al_draw_tinted_scaled_rotated_bitmap(Sprite, al_map_rgba(255, 255, 255, 255), 0, 0, GlobalPosition.x, GlobalPosition.y, Size.width / al_get_bitmap_width(Sprite), Size.height / al_get_bitmap_height(Sprite), 0, 0);
        }

        if (font)
        {
            int textWidth = al_get_text_width(font, text.c_str());
            int textHeight = al_get_font_line_height(font);

            float textX = GlobalPosition.x + (Size.width - textWidth) / 2;
            float textY = GlobalPosition.y + (Size.height - textHeight) / 2;

            if (isPressed)
            {
                al_draw_text(font, al_map_rgb(80, 80, 80), textX, textY, 0, text.c_str());
            }
            else if (MouseHover())
            {
                al_draw_text(font, al_map_rgb(120, 120, 120), textX, textY, 0, text.c_str());
            }
            else
            {
                al_draw_text(font, al_map_rgb(255, 255, 255), textX, textY, 0, text.c_str());
            }
        }

        if (MouseHover())
        {
            al_draw_rectangle(GlobalPosition.x, GlobalPosition.y, GlobalPosition.x + Size.width, GlobalPosition.y + Size.height, al_map_rgb(255, 255, 255), 2.0f);
        }


        UIElement::Draw();
    }

    void UIButton::SetOnClick(std::function<void()> callback)
    {
        onClick = callback;
    }

    void UIButton::SetText(const std::string &text)
    {
        this->text = text;
    }

    void UIButton::SetFont(ALLEGRO_FONT *font)
    {
        this->font = font;
    }

    void UIButton::SetTextColor(WAND_COLOR color)
    {
        textColor = color;
    }

    void UIButton::HandleInput()
    {
        bool wasPressed = isPressed;
        isPressed = MouseHover() && InputManager::GetInstance().IsMouseButtonPressed(1, KeyEventType::KeyDown);

        if (wasPressed && !isPressed && onClick)
        {
            onClick();
        }
    }

}
