#include "UIButton.hpp"
#include "../../../Managers/Input/InputManager.hpp"
#include "../../../Managers/Camera/CameraManager.hpp"
#include "../../../Managers/Resource/ResourceManager.hpp"

#include <iostream>

namespace WandEngine
{

    UIButton::UIButton(const std::string &text, const std::string &font, const std::string &texture, glm::vec2 Position, glm::vec2 Size, bool ReleaseMode, bool visible, UIElement *parent) : 
        text(text), font(font), texture(texture), UIElement(Position, Size, visible, parent, UIElementType::Button), textColor(glm::vec3(1.0f, 1.0f, 1.0f)), isPressed(false),
        clickOnArea(false), onClick(nullptr), ReleaseMode(ReleaseMode), cachedText(text)
    {
        auto tmp = ResourceManager::GetInstance().MeasureText(text, font, glm::vec3(1.0f));
        cachedSize.x = Position.x + (Size.x - tmp.x) / 2;
        cachedSize.y = Position.y + (Size.y - tmp.y) / 2;
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

        glm::vec3 color = glm::vec3(1.0f);

        if (MouseHover()) color = glm::vec3(0.0f, 1.0f, 0.0f);   //aplicar callbacks 

        //if (isPressed) color = glm::vec3(1.0f, 0.0f, 0.0f); //aplicar callbacks 

        if(!texture.empty())
        {
            ResourceManager::GetInstance().Render2DSprite(texture, Position, Size, 0.0f, glm::vec3(1.0f), 1.0f, ViewType::UI);
        }
        else
        {
            ResourceManager::GetInstance().RenderRectangle(
                glm::vec3(Position, 0.0f), 
                glm::vec3(Position + Size, 0.0f), 
                glm::vec3(Position.x + Size.x / 2, Position.y + Size.y / 2, 0.0f), 
                glm::vec3(0.0f, 0.0f, 0.0f), 
                color,
                1.0f, 
                1.0f,
                false,
                ViewType::UI
            );
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

            ResourceManager::GetInstance().RenderText(text, font, glm::vec3(cachedSize.x, cachedSize.y, 1.0f), glm::vec3(1.0f), glm::vec3(), glm::vec3(0.0f), color, 1.0f, ViewType::UI);
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

    void UIButton::SetFont(const std::string &font)
    {
        this->font = font;
    }

    void UIButton::SetTextColor(glm::vec3 color)
    {
        textColor = color;
    }

    void UIButton::HandleInput()
    {
        bool MD = InputManager::GetInstance().IsMouseButtonPressed(0, KeyEventType::KeyDown);
        bool isHovering = MouseHover();
    
        isPressed = false;
    
        if (MD && isHovering)
        {
            clickOnArea = true;
        }
    
        if (ReleaseMode)
        {
            bool MR = InputManager::GetInstance().IsMouseButtonPressed(0, KeyEventType::KeyRelease);

            if (MR && isHovering && clickOnArea && onClick)
            {
                isPressed = true;
                onClick();
            }
    
            if (!MR || !isHovering)
            {
                clickOnArea = false;
            }
        }
        else
        {
            if (MD && isHovering && clickOnArea && onClick)
            {
                isPressed = true;
                onClick();
            }

        }

    }
    
    
    
    

}
