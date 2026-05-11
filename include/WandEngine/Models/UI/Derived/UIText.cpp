#include "UIText.hpp"
#include "../../../Managers/Input/InputManager.hpp"
#include "../../../Managers/Camera/CameraManager.hpp"
#include "../../../Managers/Resource/ResourceManager.hpp"
#include <iostream>

namespace WandEngine
{

    UIText::UIText(const std::string &text, const std::string &font, glm::vec2 Position, glm::vec2 Size, bool visible, UIElement *parent) : 
        text(text), font(font), UIElement(Position, Size, visible, parent, UIElementType::Label), textColor(glm::vec3(1.0f))
    {
        auto tmp = ResourceManager::GetInstance().MeasureText(text, font, glm::vec3(1.0f));
        cachedSize.x = Position.x + (Size.x - tmp.x) / 2;
        cachedSize.y = Position.y + (Size.y - tmp.y) / 2;
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

        if(!text.empty() && !font.empty())
        {
            if (text != cachedText)
            {
                cachedText = text;
                auto tmp = ResourceManager::GetInstance().MeasureText(text, font, glm::vec3(1.0f));
                cachedSize.x = Position.x + (Size.x - tmp.x) / 2;
                cachedSize.y = Position.y + (Size.y - tmp.y) / 2;
            }
    
            ResourceManager::GetInstance().RenderText(text, font, glm::vec3(cachedSize.x, cachedSize.y, 1.0f), glm::vec3(1.0f), glm::vec3(), glm::vec3(0.0f), textColor, 1.0f, ViewType::UI);
        }
        
        UIElement::Draw();
    }


    void UIText::SetText(const std::string &text)
    {
        this->text = text;
    }

    void UIText::SetFont(const std::string &font)
    {
        this->font = font;
    }

    void UIText::SetTextColor(glm::vec3 color)
    {
        textColor = color;
    }


}
