#ifndef COSMIC_UITEXTAREA_HPP
#define COSMIC_UITEXTAREA_HPP

#include <glm/glm.hpp>
#include "../ui_element.hpp"
#include <functional>
#include <string>
#include <vector>

namespace CosmicEngine
{
    // Multiline text input element.
    // - Enter inserts a newline character.
    // - Cursor is always at end of text (simplified; no arrow-key navigation across lines).
    // - Vertical scrolling keeps the last line visible.
    class UITextArea : public UIElement
    {
    private:
        std::string text;
        std::string font;

        glm::vec3 textColor;
        glm::vec3 focusTextColor;
        glm::vec3 borderColor;
        glm::vec3 focusBorderColor;
        glm::vec3 backgroundColor;

        bool focused;
        int maxLength;
        float cursorBlinkTimer;
        bool cursorVisible;
        float backspaceHoldTimer;
        float backspaceRepeatTimer;
        float scrollOffsetY;

        std::function<void(const std::string &)> onTextChanged;

        void HandleInput(float deltaTime);
        void ResetCursorBlink();
        void NotifyTextChanged();
        void AppendCharacter(unsigned int codepoint);
        void RemoveLastCharacter();
        std::vector<std::string> SplitLines() const;
        std::vector<std::string> GetVisualLines() const;
        float GetLineHeight() const;
        void ClampScroll();

    public:
        UITextArea(const std::string &initialText,
                   const std::string &font,
                   glm::vec2 Position,
                   glm::vec2 Size,
                   int maxLength = 4096,
                   bool visible = true,
                   UIElement *parent = nullptr);
        virtual ~UITextArea();

        void update(float deltaTime) override;
        void draw() override;
        bool IsFocusable() const override;
        void Activate() override;

        void SetText(const std::string &text);
        std::string GetText() const;

        void SetFont(const std::string &font);
        void SetMaxLength(int maxLength);

        void SetTextColor(glm::vec3 color);
        void SetFocusTextColor(glm::vec3 color);
        void SetBorderColor(glm::vec3 color);
        void SetFocusBorderColor(glm::vec3 color);
        void SetBackgroundColor(glm::vec3 color);

        void SetFocused(bool focused);
        bool IsFocused() const;

        void SetOnTextChanged(std::function<void(const std::string &)> callback);
    };
}

#endif // COSMIC_UITEXTAREA_HPP
