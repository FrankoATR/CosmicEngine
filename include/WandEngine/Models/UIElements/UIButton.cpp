#include "UIButton.hpp"
#include "../../Managers/Input/InputManager.hpp"
#include "../../Managers/Camera/CameraManager.hpp"

#include <iostream>

namespace WandEngine
{

    UIButton::UIButton(unsigned int *Sprite, const std::string &text, int *font, glm::vec2 Position, glm::vec2 Size, bool visible, UIElement *parent) : 
        Sprite(Sprite), text(text), font(font), UIElement(Position, Size, visible, parent, UIElementType::Button), textColor(glm::vec3(255, 255, 255)), isPressed(false)
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
            
        }

        if (font)
        {
            int textWidth = 1;
            int textHeight = 2;

            float textX = GlobalPosition.x + (Size.x - textWidth) / 2;
            float textY = GlobalPosition.y + (Size.y - textHeight) / 2;

            if (isPressed)
            {
                //al_draw_text(font, al_map_rgb(80, 80, 80), textX, textY, 0, text.c_str());
            }
            else if (MouseHover())
            {
                //al_draw_text(font, al_map_rgb(120, 120, 120), textX, textY, 0, text.c_str());
            }
            else
            {
                //al_draw_text(font, al_map_rgb(255, 255, 255), textX, textY, 0, text.c_str());
            }
        }

        if (MouseHover())
        {
            //al_draw_rectangle(GlobalPosition.x, GlobalPosition.y, GlobalPosition.x + Size.width, GlobalPosition.y + Size.height, al_map_rgb(255, 255, 255), 2.0f);
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

    void UIButton::SetFont(int *font)
    {
        this->font = font;
    }

    void UIButton::SetTextColor(glm::vec3 color)
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
