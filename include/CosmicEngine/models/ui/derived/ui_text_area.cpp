#include "ui_text_area.hpp"
#include "../../../managers/input/input_manager.hpp"
#include "../../../managers/resource/resource_manager.hpp"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <sstream>

namespace CosmicEngine
{
    namespace
    {
        constexpr float kBackspaceRepeatDelay    = 0.65f;
        constexpr float kBackspaceRepeatInterval = 0.06f;
        constexpr float kTextPadding             = 8.0f;

        std::string EncodeUtf8(char32_t codepoint)
        {
            std::string result;
            if (codepoint <= 0x7F)
            {
                result.push_back(static_cast<char>(codepoint));
            }
            else if (codepoint <= 0x7FF)
            {
                result.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
                result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
            }
            else if (codepoint <= 0xFFFF)
            {
                result.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
            }
            else
            {
                result.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
            }
            return result;
        }

        // Count UTF-8 codepoints (excluding null terminator).
        std::size_t Utf8CodepointCount(const std::string &s)
        {
            std::size_t count = 0;
            for (std::size_t i = 0; i < s.size();)
            {
                const unsigned char lead = static_cast<unsigned char>(s[i]);
                std::size_t advance = 1;
                if ((lead & 0xE0) == 0xC0 && i + 1 < s.size()) advance = 2;
                else if ((lead & 0xF0) == 0xE0 && i + 2 < s.size()) advance = 3;
                else if ((lead & 0xF8) == 0xF0 && i + 3 < s.size()) advance = 4;
                i += advance;
                ++count;
            }
            return count;
        }

        // Remove the last UTF-8 codepoint from a string in-place. Returns true if removed.
        bool RemoveLastUtf8Codepoint(std::string &s)
        {
            if (s.empty()) return false;
            // Walk backwards to find the start of the last codepoint.
            std::size_t i = s.size();
            --i;
            while (i > 0 && (static_cast<unsigned char>(s[i]) & 0xC0) == 0x80)
                --i;
            s.erase(i);
            return true;
        }
    }

    UITextArea::UITextArea(const std::string &initialText,
                           const std::string &font,
                           glm::vec2 Position,
                           glm::vec2 Size,
                           int maxLength,
                           bool visible,
                           UIElement *parent)
        : UIElement(Position, Size, visible, parent, UIElementType::TextArea),
          text(initialText),
          font(font),
          textColor(glm::vec3(1.0f)),
          focusTextColor(glm::vec3(0.6f, 0.9f, 1.0f)),
          borderColor(glm::vec3(0.6f)),
          focusBorderColor(glm::vec3(0.2f, 0.7f, 1.0f)),
          backgroundColor(glm::vec3(0.15f)),
          focused(false),
          maxLength(maxLength),
          cursorBlinkTimer(0.0f),
          cursorVisible(true),
          backspaceHoldTimer(0.0f),
          backspaceRepeatTimer(0.0f),
          scrollOffsetY(0.0f)
    {
    }

    UITextArea::~UITextArea() {}

    std::vector<std::string> UITextArea::SplitLines() const
    {
        std::vector<std::string> lines;
        std::istringstream stream(text);
        std::string line;
        while (std::getline(stream, line, '\n'))
            lines.push_back(line);
        // Preserve a trailing empty line when text ends with '\n'.
        if (!text.empty() && text.back() == '\n')
            lines.push_back("");
        return lines;
    }

    std::vector<std::string> UITextArea::GetVisualLines() const
    {
        const float maxWidth = Size.x - kTextPadding * 2.0f;
        auto &rs = ResourceManager::GetInstance();
        std::vector<std::string> result;

        for (const auto &logLine : SplitLines())
        {
            if (logLine.empty()) { result.push_back(""); continue; }

            auto sz = rs.MeasureText(logLine, font, glm::vec3(1.0f));
            if (sz.x <= maxWidth) { result.push_back(logLine); continue; }

            // Word-wrap: split on spaces and accumulate words until the line overflows.
            std::istringstream iss(logLine);
            std::string word, current;
            while (iss >> word)
            {
                std::string candidate = current.empty() ? word : current + " " + word;
                auto csz = rs.MeasureText(candidate, font, glm::vec3(1.0f));
                if (csz.x > maxWidth && !current.empty())
                {
                    result.push_back(current);
                    current = word;
                }
                else
                {
                    current = candidate;
                }
            }
            if (!current.empty()) result.push_back(current);
        }
        return result;
    }

    float UITextArea::GetLineHeight() const
    {
        if (font.empty()) return 18.0f;
        auto sz = ResourceManager::GetInstance().MeasureText("Ag", font, glm::vec3(1.0f));
        return sz.y + 4.0f;
    }

    void UITextArea::ClampScroll()
    {
        auto lines = GetVisualLines();
        float lineH  = GetLineHeight();
        float contentH = static_cast<float>(lines.size()) * lineH;
        float visibleH = Size.y - kTextPadding * 2.0f;
        float maxScroll = std::max(0.0f, contentH - visibleH);
        scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, maxScroll);
    }

    void UITextArea::update(float deltaTime)
    {
        // Click-to-focus.
        if (InputManager::GetInstance().IsMouseButtonPressed(0, KeyEventType::KeyDown))
        {
            bool wasFocused = focused;
            focused = MouseHover();
            if (focused && !wasFocused)
                InputManager::GetInstance().ClearCharacterInput();
            if (!focused)
            {
                backspaceHoldTimer = 0.0f;
                backspaceRepeatTimer = 0.0f;
                InputManager::GetInstance().ClearCharacterInput();
            }
        }

        if (focused)
        {
            HandleInput(deltaTime);
            cursorBlinkTimer += deltaTime;
            if (cursorBlinkTimer >= 0.5f)
            {
                cursorVisible = !cursorVisible;
                cursorBlinkTimer = 0.0f;
            }
        }

        UIElement::update(deltaTime);
    }

    void UITextArea::HandleInput(float deltaTime)
    {
        auto &input = InputManager::GetInstance();

        // Backspace with hold-repeat.
        if (input.IsKeyPressed(GLFW_KEY_BACKSPACE, KeyEventType::KeyDown))
        {
            RemoveLastCharacter();
            backspaceHoldTimer = 0.0f;
            backspaceRepeatTimer = 0.0f;
        }
        else if (input.IsKeyPressed(GLFW_KEY_BACKSPACE, KeyEventType::KeyRelease))
        {
            backspaceHoldTimer += deltaTime;
            if (backspaceHoldTimer >= kBackspaceRepeatDelay)
            {
                backspaceRepeatTimer += deltaTime;
                while (backspaceRepeatTimer >= kBackspaceRepeatInterval)
                {
                    RemoveLastCharacter();
                    backspaceRepeatTimer -= kBackspaceRepeatInterval;
                }
            }
        }
        else
        {
            backspaceHoldTimer = 0.0f;
            backspaceRepeatTimer = 0.0f;
        }

        // Enter inserts a newline.
        if (input.IsKeyPressed(GLFW_KEY_ENTER, KeyEventType::KeyDown))
        {
            if (static_cast<int>(Utf8CodepointCount(text)) < maxLength)
            {
                text += '\n';
                NotifyTextChanged();
            }
        }

        // Escape unfocuses.
        if (input.IsKeyPressed(GLFW_KEY_ESCAPE, KeyEventType::KeyDown))
        {
            focused = false;
            backspaceHoldTimer = 0.0f;
            backspaceRepeatTimer = 0.0f;
            input.ClearCharacterInput();
            return;
        }

        // Printable characters.
        unsigned int codepoint = 0;
        while (input.PollCharacterInput(codepoint))
        {
            if (static_cast<int>(Utf8CodepointCount(text)) >= maxLength) break;
            if (codepoint >= 32 && codepoint != 127)
                AppendCharacter(codepoint);
        }

        ClampScroll();
        // Auto-scroll to bottom so the cursor is always visible.
        auto lines = GetVisualLines();
        float lineH     = GetLineHeight();
        float contentH  = static_cast<float>(lines.size()) * lineH;
        float visibleH  = Size.y - kTextPadding * 2.0f;
        float maxScroll = std::max(0.0f, contentH - visibleH);
        scrollOffsetY = maxScroll;
    }

    void UITextArea::ResetCursorBlink()
    {
        cursorBlinkTimer = 0.0f;
        cursorVisible = true;
    }

    void UITextArea::NotifyTextChanged()
    {
        ResetCursorBlink();
        if (onTextChanged)
            onTextChanged(text);
    }

    void UITextArea::AppendCharacter(unsigned int codepoint)
    {
        text += EncodeUtf8(static_cast<char32_t>(codepoint));
        NotifyTextChanged();
    }

    void UITextArea::RemoveLastCharacter()
    {
        if (RemoveLastUtf8Codepoint(text))
            NotifyTextChanged();
    }

    bool UITextArea::IsFocusable() const { return true; }

    void UITextArea::Activate() { SetFocused(true); }

    void UITextArea::draw()
    {
        if (!visible) return;

        auto &rs = ResourceManager::GetInstance();

        // Background.
        rs.RenderRectangle(
            Position, Position + Size,
            glm::vec2(Position.x + Size.x / 2, Position.y + Size.y / 2),
            glm::vec2(0.0f),
            backgroundColor, 0.9f, 1.0f, true, ViewType::UI);

        // Border.
        glm::vec3 currentBorder = focused ? focusBorderColor : borderColor;
        rs.RenderRectangle(
            Position, Position + Size,
            glm::vec2(Position.x + Size.x / 2, Position.y + Size.y / 2),
            glm::vec2(0.0f),
            currentBorder, 1.0f, 2.0f, false, ViewType::UI);

        if (font.empty()) { UIElement::draw(); return; }

        glm::vec3 currentTextColor = focused ? focusTextColor : textColor;
        float lineH = GetLineHeight();
        auto lines  = GetVisualLines();

        float clipMinY = Position.y + kTextPadding;
        float clipMaxY = Position.y + Size.y - kTextPadding;

        for (std::size_t i = 0; i < lines.size(); ++i)
        {
            float lineTop = Position.y + kTextPadding + static_cast<float>(i) * lineH - scrollOffsetY;
            float lineBot = lineTop + lineH;

            // Skip completely out-of-view lines.
            if (lineBot < clipMinY || lineTop > clipMaxY) continue;

            if (!lines[i].empty())
            {
                float textX = Position.x + kTextPadding;
                float textY = lineTop;
                rs.RenderText(lines[i], font,
                              glm::vec3(textX, textY, 1.0f),
                              glm::vec3(1.0f), glm::vec3(), glm::vec3(0.0f),
                              currentTextColor, 1.0f, ViewType::UI);
            }
        }

        // Draw cursor at end of last line.
        if (focused && cursorVisible && !lines.empty())
        {
            std::size_t lastIdx = lines.size() - 1;
            float lineTop = Position.y + kTextPadding + static_cast<float>(lastIdx) * lineH - scrollOffsetY;
            float cursorX = Position.x + kTextPadding;
            if (!lines[lastIdx].empty())
            {
                auto sz = rs.MeasureText(lines[lastIdx], font, glm::vec3(1.0f));
                cursorX += sz.x;
            }
            float cursorY1 = lineTop + 3.0f;
            float cursorY2 = lineTop + lineH - 3.0f;
            cursorY1 = std::max(cursorY1, clipMinY);
            cursorY2 = std::min(cursorY2, clipMaxY);
            if (cursorY2 > cursorY1)
            {
                rs.RenderRectangle(
                    glm::vec2(cursorX, cursorY1),
                    glm::vec2(cursorX + 2.0f, cursorY2),
                    glm::vec2(cursorX + 1.0f, (cursorY1 + cursorY2) * 0.5f),
                    glm::vec2(0.0f),
                    currentTextColor, 1.0f, 1.0f, true, ViewType::UI);
            }
        }

        UIElement::draw();
    }

    void UITextArea::SetText(const std::string &newText)
    {
        text = newText;
        ClampScroll();
    }

    std::string UITextArea::GetText() const { return text; }

    void UITextArea::SetFont(const std::string &f) { font = f; }
    void UITextArea::SetMaxLength(int ml) { maxLength = ml; }
    void UITextArea::SetTextColor(glm::vec3 color) { textColor = color; }
    void UITextArea::SetFocusTextColor(glm::vec3 color) { focusTextColor = color; }
    void UITextArea::SetBorderColor(glm::vec3 color) { borderColor = color; }
    void UITextArea::SetFocusBorderColor(glm::vec3 color) { focusBorderColor = color; }
    void UITextArea::SetBackgroundColor(glm::vec3 color) { backgroundColor = color; }

    void UITextArea::SetFocused(bool f)
    {
        UIElement::SetFocused(f);
        focused = f;
        if (f)
            ResetCursorBlink();
        else
        {
            backspaceHoldTimer = 0.0f;
            backspaceRepeatTimer = 0.0f;
        }
    }

    bool UITextArea::IsFocused() const { return focused; }

    void UITextArea::SetOnTextChanged(std::function<void(const std::string &)> callback)
    {
        onTextChanged = std::move(callback);
    }
}
