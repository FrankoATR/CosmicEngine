#include "UIText.hpp"
#include "../../Managers/Input/InputManager.hpp"
#include "../../Managers/Camera/CameraManager.hpp"
#include <iostream>

namespace WandEngine
{

    UIText::UIText(const std::string &text, int *font, glm::vec2 Position, glm::vec2 Size, bool visible, UIElement *parent) : 
        text(text), font(font), UIElement(Position, Size, visible, parent, UIElementType::Label), textColor(glm::vec3(1.0f))
    {
    }

    UIText::~UIText()
    {
    }

    void UIText::Update(float deltaTime)
    {
        UIElement::Update(deltaTime);
    }

    void UIText::Draw()
    {
        if (!visible)
        {
            return;
        }

        if (font)
        {
            int textWidth = 1;
            int textHeight = 2;

            float textX = GlobalPosition.x + (Size.x - textWidth) / 2;
            float textY = GlobalPosition.y + (Size.y - textHeight) / 2;

        }

        UIElement::Draw();
    }


    void UIText::SetText(const std::string &text)
    {
        this->text = text;
    }

    void UIText::SetFont(int *font)
    {
        this->font = font;
    }

    void UIText::SetTextColor(glm::vec3 color)
    {
        textColor = color;
    }


}
