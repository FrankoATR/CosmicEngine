#include "ui_text_field.hpp"
#include "../../../managers/input/input_manager.hpp"
#include "../../../managers/camera/camera_manager.hpp"
#include "../../../managers/resource/resource_manager.hpp"
#include "../../../managers/game_manager.hpp"

#include <iostream>
#include <algorithm>

namespace CosmicEngine
{

    UITextField::UITextField(const std::string &initialText, const std::string &placeholder, const std::string &font,
                             glm::vec2 Position, glm::vec2 Size, int maxLength, bool visible, UIElement *parent)
        : UIElement(Position, Size, visible, parent, UIElementType::TextField),
          text(initialText), placeholder(placeholder), font(font),
          textColor(glm::vec3(1.0f)),
            focusTextColor(glm::vec3(0.6f, 0.9f, 1.0f)),
          placeholderColor(glm::vec3(0.5f)),
          borderColor(glm::vec3(0.6f)),
          focusBorderColor(glm::vec3(0.2f, 0.7f, 1.0f)),
          backgroundColor(glm::vec3(0.15f)),
          focused(false), maxLength(maxLength),
            cursorIndex(initialText.size()),
          cursorBlinkTimer(0.0f), cursorVisible(true),
            backspaceHoldTimer(0.0f), backspaceRepeatTimer(0.0f),
          onSubmit(nullptr), onTextChanged(nullptr)
    {
    }

    UITextField::~UITextField()
    {
    }

    void UITextField::update(float deltaTime)
    {
        // Handle focus by click
        if (InputManager::GetInstance().IsMouseButtonPressed(0, KeyEventType::KeyDown))
        {
            bool wasFocused = focused;
            focused = MouseHover();
            if (focused)
            {
                if (!wasFocused)
                {
                    InputManager::GetInstance().ClearCharacterInput();
                }

                ResetCursorBlink();
            }
            else
            {
                backspaceHoldTimer = 0.0f;
                backspaceRepeatTimer = 0.0f;
                InputManager::GetInstance().ClearCharacterInput();
            }
        }

        if (focused)
        {
            HandleInput(deltaTime);

            // Blink cursor
            cursorBlinkTimer += deltaTime;
            if (cursorBlinkTimer >= 0.5f)
            {
                cursorVisible = !cursorVisible;
                cursorBlinkTimer = 0.0f;
            }
        }

        UIElement::update(deltaTime);
    }

    void UITextField::HandleInput(float deltaTime)
    {
        auto &input = InputManager::GetInstance();

        // Handle backspace
        if (input.IsKeyPressed(GLFW_KEY_BACKSPACE, KeyEventType::KeyDown))
        {
            RemoveCharacterBeforeCursor();
            backspaceHoldTimer = 0.0f;
            backspaceRepeatTimer = 0.0f;
        }
        else if (input.IsKeyPressed(GLFW_KEY_BACKSPACE, KeyEventType::KeyRelease))
        {
            backspaceHoldTimer += deltaTime;
            if (backspaceHoldTimer >= 1.0f)
            {
                backspaceRepeatTimer += deltaTime;
                constexpr float kBackspaceRepeatInterval = 0.06f;
                while (backspaceRepeatTimer >= kBackspaceRepeatInterval)
                {
                    RemoveCharacterBeforeCursor();
                    backspaceRepeatTimer -= kBackspaceRepeatInterval;
                }
            }
        }
        else
        {
            backspaceHoldTimer = 0.0f;
            backspaceRepeatTimer = 0.0f;
        }

        if (input.IsKeyPressed(GLFW_KEY_LEFT, KeyEventType::KeyDown))
        {
            if (cursorIndex > 0)
            {
                --cursorIndex;
                ResetCursorBlink();
            }
        }

        if (input.IsKeyPressed(GLFW_KEY_RIGHT, KeyEventType::KeyDown))
        {
            if (cursorIndex < text.size())
            {
                ++cursorIndex;
                ResetCursorBlink();
            }
        }

        // Handle enter/submit
        if (input.IsKeyPressed(GLFW_KEY_ENTER, KeyEventType::KeyDown))
        {
            if (onSubmit)
                onSubmit(text);
        }

        // Handle escape to unfocus
        if (input.IsKeyPressed(GLFW_KEY_ESCAPE, KeyEventType::KeyDown))
        {
            focused = false;
            backspaceHoldTimer = 0.0f;
            backspaceRepeatTimer = 0.0f;
            input.ClearCharacterInput();
            return;
        }

        unsigned int codepoint = 0;
        while (input.PollCharacterInput(codepoint))
        {
            if (static_cast<int>(text.size()) >= maxLength)
            {
                break;
            }

            if (codepoint >= 32 && codepoint <= 126)
            {
                InsertCharacter(static_cast<char>(codepoint));
            }
        }
    }

    void UITextField::ResetCursorBlink()
    {
        cursorBlinkTimer = 0.0f;
        cursorVisible = true;
    }

    void UITextField::NotifyTextChanged()
    {
        if (onTextChanged)
        {
            onTextChanged(text);
        }
        ResetCursorBlink();
    }

    void UITextField::InsertCharacter(char character)
    {
        if (static_cast<int>(text.size()) >= maxLength)
        {
            return;
        }

        text.insert(text.begin() + static_cast<std::string::difference_type>(cursorIndex), character);
        ++cursorIndex;
        NotifyTextChanged();
    }

    void UITextField::RemoveCharacterBeforeCursor()
    {
        if (cursorIndex == 0 || text.empty())
        {
            return;
        }

        text.erase(text.begin() + static_cast<std::string::difference_type>(cursorIndex - 1));
        --cursorIndex;
        NotifyTextChanged();
    }

    glm::vec3 UITextField::GetCurrentTextColor() const
    {
        return focused ? focusTextColor : textColor;
    }

    float UITextField::GetCursorOffsetX() const
    {
        if (font.empty() || cursorIndex == 0)
        {
            return 0.0f;
        }

        return ResourceManager::GetInstance().MeasureText(text.substr(0, cursorIndex), font, glm::vec3(1.0f)).x;
    }

    void UITextField::draw()
    {
        if (!visible)
            return;

        auto &rs = ResourceManager::GetInstance();

        // Draw background
        glm::vec3 currentBorder = focused ? focusBorderColor : borderColor;

        rs.RenderRectangle(
            Position,
            Position + Size,
            glm::vec2(Position.x + Size.x / 2, Position.y + Size.y / 2),
            glm::vec2(0.0f),
            backgroundColor,
            0.9f,
            1.0f,
            true,
            ViewType::UI);

        // Draw border
        rs.RenderRectangle(
            Position,
            Position + Size,
            glm::vec2(Position.x + Size.x / 2, Position.y + Size.y / 2),
            glm::vec2(0.0f),
            currentBorder,
            1.0f,
            2.0f,
            false,
            ViewType::UI);

        // Draw text or placeholder
        float textPadding = 8.0f;
        glm::vec2 textPos(Position.x + textPadding, Position.y);
        glm::vec3 currentTextColor = GetCurrentTextColor();
        float cursorX = textPos.x + GetCursorOffsetX();
        float cursorY1 = Position.y + 6.0f;
        float cursorY2 = Position.y + Size.y - 6.0f;
        glm::vec2 cursorMin(cursorX, cursorY1);
        glm::vec2 cursorMax(cursorX + 2.0f, cursorY2);
        glm::vec2 cursorPivot(cursorX + 1.0f, (cursorY1 + cursorY2) * 0.5f);

        if (!text.empty() && !font.empty())
        {
            auto textSize = rs.MeasureText(text, font, glm::vec3(1.0f));
            float yOffset = (Size.y - textSize.y) / 2.0f;
            rs.RenderText(text, font, glm::vec3(textPos.x, Position.y + yOffset, 1.0f),
                          glm::vec3(1.0f), glm::vec3(), glm::vec3(0.0f), currentTextColor, 1.0f, ViewType::UI);

            // Draw cursor
            if (focused && cursorVisible)
            {
                rs.RenderRectangle(
                    cursorMin,
                    cursorMax,
                    cursorPivot,
                    glm::vec2(0.0f),
                    currentTextColor,
                    1.0f,
                    1.0f,
                    false,
                    ViewType::UI);
            }
        }
        else if (!placeholder.empty() && !font.empty())
        {
            auto placeholderSize = rs.MeasureText(placeholder, font, glm::vec3(1.0f));
            float yOffset = (Size.y - placeholderSize.y) / 2.0f;
            rs.RenderText(placeholder, font, glm::vec3(textPos.x, Position.y + yOffset, 1.0f),
                          glm::vec3(1.0f), glm::vec3(), glm::vec3(0.0f), placeholderColor, 0.7f, ViewType::UI);

            // Draw cursor at start if focused
            if (focused && cursorVisible)
            {
                rs.RenderRectangle(
                    cursorMin,
                    cursorMax,
                    cursorPivot,
                    glm::vec2(0.0f),
                    currentTextColor,
                    1.0f,
                    1.0f,
                    false,
                    ViewType::UI);
            }
        }
        else if (focused && cursorVisible)
        {
            rs.RenderRectangle(
                cursorMin,
                cursorMax,
                cursorPivot,
                glm::vec2(0.0f),
                currentTextColor,
                1.0f,
                1.0f,
                false,
                ViewType::UI);
        }

        UIElement::draw();
    }

    void UITextField::SetText(const std::string &text)
    {
        this->text = text;
        cursorIndex = std::min(cursorIndex, this->text.size());
    }

    std::string UITextField::GetText() const
    {
        return text;
    }

    void UITextField::SetPlaceholder(const std::string &placeholder)
    {
        this->placeholder = placeholder;
    }

    void UITextField::SetFont(const std::string &font)
    {
        this->font = font;
    }

    void UITextField::SetMaxLength(int maxLength)
    {
        this->maxLength = maxLength;
    }

    void UITextField::SetTextColor(glm::vec3 color)
    {
        textColor = color;
    }

    void UITextField::SetFocusTextColor(glm::vec3 color)
    {
        focusTextColor = color;
    }

    void UITextField::SetPlaceholderColor(glm::vec3 color)
    {
        placeholderColor = color;
    }

    void UITextField::SetBorderColor(glm::vec3 color)
    {
        borderColor = color;
    }

    void UITextField::SetFocusBorderColor(glm::vec3 color)
    {
        focusBorderColor = color;
    }

    void UITextField::SetBackgroundColor(glm::vec3 color)
    {
        backgroundColor = color;
    }

    void UITextField::SetFocused(bool focused)
    {
        this->focused = focused;
        if (focused)
        {
            cursorIndex = std::min(cursorIndex, text.size());
            ResetCursorBlink();
        }
        else
        {
            backspaceHoldTimer = 0.0f;
            backspaceRepeatTimer = 0.0f;
        }
    }

    bool UITextField::IsFocused() const
    {
        return focused;
    }

    void UITextField::SetOnSubmit(std::function<void(const std::string &)> callback)
    {
        onSubmit = std::move(callback);
    }

    void UITextField::SetOnTextChanged(std::function<void(const std::string &)> callback)
    {
        onTextChanged = std::move(callback);
    }

}
