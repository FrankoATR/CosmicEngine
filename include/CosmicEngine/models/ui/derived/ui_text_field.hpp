#ifndef COSMIC_UITEXTFIELD_HPP
#define COSMIC_UITEXTFIELD_HPP

#include <glm/glm.hpp>
#include "../ui_element.hpp"
#include <functional>
#include <string>

namespace CosmicEngine
{

    /**
     * @class UITextField
     * @brief A UI element that allows text input from the keyboard.
     * 
     * Usage:
     * @code
     * auto* textField = new UITextField("", "placeholder text", "my_font", {100, 100}, {300, 40});
     * textField->SetOnSubmit([](const std::string& text) { std::cout << text << std::endl; });
     * UIManager::GetInstance().AddElement(textField);
     * @endcode
     */
    class UITextField : public UIElement
    {
    private:
        std::string text;
        std::string placeholder;
        std::string font;

        glm::vec3 textColor;
        glm::vec3 focusTextColor;
        glm::vec3 placeholderColor;
        glm::vec3 borderColor;
        glm::vec3 focusBorderColor;
        glm::vec3 backgroundColor;

        bool focused;
        int maxLength;
        std::size_t cursorIndex;
        float cursorBlinkTimer;
        bool cursorVisible;
        float backspaceHoldTimer;
        float backspaceRepeatTimer;

        std::function<void(const std::string &)> onSubmit;
        std::function<void(const std::string &)> onTextChanged;

        void HandleInput(float deltaTime);
        void ResetCursorBlink();
        void NotifyTextChanged();
        void InsertCharacter(char character);
        void RemoveCharacterBeforeCursor();
        glm::vec3 GetCurrentTextColor() const;
        float GetCursorOffsetX() const;

    public:
        UITextField(const std::string &initialText, const std::string &placeholder, const std::string &font,
                    glm::vec2 Position, glm::vec2 Size, int maxLength = 256, bool visible = true, UIElement *parent = nullptr);
        virtual ~UITextField();

        void update(float deltaTime) override;
        void draw() override;

        void SetText(const std::string &text);
        std::string GetText() const;

        void SetPlaceholder(const std::string &placeholder);
        void SetFont(const std::string &font);
        void SetMaxLength(int maxLength);

        void SetTextColor(glm::vec3 color);
        void SetFocusTextColor(glm::vec3 color);
        void SetPlaceholderColor(glm::vec3 color);
        void SetBorderColor(glm::vec3 color);
        void SetFocusBorderColor(glm::vec3 color);
        void SetBackgroundColor(glm::vec3 color);

        void SetFocused(bool focused);
        bool IsFocused() const;

        void SetOnSubmit(std::function<void(const std::string &)> callback);
        void SetOnTextChanged(std::function<void(const std::string &)> callback);
    };

}

#endif // COSMIC_UITEXTFIELD_HPP
