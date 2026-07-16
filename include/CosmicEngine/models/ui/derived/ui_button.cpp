#include "ui_button.hpp"
#include "../../../managers/input/input_manager.hpp"
#include "../../../managers/camera/camera_manager.hpp"
#include "../../../managers/resource/resource_manager.hpp"

#include <iostream>

namespace CosmicEngine
{

    UIButton::UIButton(const std::string &text, const std::string &font, const std::string &texture, glm::vec2 Position, glm::vec2 Size, bool ReleaseMode, bool visible, UIElement *parent) : 
        text(text), font(font), texture(texture), hoverTexture(), UIElement(Position, Size, visible, parent, UIElementType::Button), textColor(glm::vec3(1.0f, 1.0f, 1.0f)), selectedTextColor(glm::vec3(1.0f, 0.9f, 0.0f)), isPressed(false),
        clickOnArea(false), onClick(nullptr), ReleaseMode(ReleaseMode), cachedText(text), textOffset(0.0f), textureScale(1.0f)
    {
        auto tmp = ResourceManager::GetInstance().MeasureText(text, font, glm::vec3(1.0f));
        cachedSize.x = Position.x + (Size.x - tmp.x) / 2;
        cachedSize.y = Position.y + (Size.y - tmp.y) / 2;
    }

    UIButton::~UIButton()
    {
    }

    void UIButton::update(float deltaTime)
    {
        HandleInput();

        UIElement::update(deltaTime);
    }

    void UIButton::draw()
    {
        if (!visible)
        {
            return;
        }

        glm::vec3 color = textColor;

        // unify visuals: both keyboard focus and mouse hover use the selected color
        if (IsFocused() || MouseHover()) color = selectedTextColor;

        // choose which texture to show (hover/focus takes precedence)
        std::string drawTexture = texture;
        if ((MouseHover() || IsFocused()) && !hoverTexture.empty()) drawTexture = hoverTexture;

        // calculate texture draw rect (allow scaling to make texture slightly larger)
        glm::vec2 drawPos = Position;
        glm::vec2 drawSize = Size;
        if (textureScale != 1.0f)
        {
            drawSize = Size * textureScale;
            drawPos = Position - (drawSize - Size) / 2.0f;
        }

        if(!drawTexture.empty())
        {
            ResourceManager::GetInstance().Render2DSpriteUnlit(drawTexture, drawPos, drawSize, 0.0f, glm::vec3(1.0f), 1.0f, ViewType::UI);
        }
        else
        {
            ResourceManager::GetInstance().RenderRectangle(
                drawPos,
                drawPos + drawSize,
                glm::vec2(drawPos.x + drawSize.x / 2, drawPos.y + drawSize.y / 2),
                glm::vec2(0.0f, 0.0f),
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
                // center text relative to the drawn texture rectangle (drawPos/drawSize)
                cachedSize.x = drawPos.x + (drawSize.x - tmp.x) / 2 + textOffset.x;
                cachedSize.y = drawPos.y + (drawSize.y - tmp.y) / 2 + textOffset.y;
            }

            ResourceManager::GetInstance().RenderText(text, font, glm::vec3(cachedSize.x, cachedSize.y, 1.0f), glm::vec3(1.0f), glm::vec3(), glm::vec3(0.0f), color, 1.0f, ViewType::UI);
        }

        UIElement::draw();
    }

    bool UIButton::IsFocusable() const
    {
        return true;
    }

    void UIButton::Activate()
    {
        if (onClick)
        {
            isPressed = true;
            onClick();
        }
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

    void UIButton::SetHoverTexture(const std::string &hoverTexture)
    {
        this->hoverTexture = hoverTexture;
    }

    void UIButton::SetTextureScale(float scale)
    {
        this->textureScale = scale > 0.0f ? scale : 1.0f;
    }

    void UIButton::SetTextOffset(const glm::vec2 &offset)
    {
        this->textOffset = offset;
    }

    void UIButton::SetTextColor(glm::vec3 color)
    {
        textColor = color;
    }

    void UIButton::SetSelectedTextColor(glm::vec3 color)
    {
        selectedTextColor = color;
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
            bool MR = InputManager::GetInstance().IsMouseButtonPressed(0, KeyEventType::KeyUp);

            if (MR && isHovering && clickOnArea && onClick)
            {
                isPressed = true;
                onClick();
            }

            if (MR || !isHovering)
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

            if (!MD || !isHovering)
            {
                clickOnArea = false;
            }

        }

    }
    
    
    
    

}
