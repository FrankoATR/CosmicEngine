#ifndef COSMIC_UITEXTFIELD_HPP
#define COSMIC_UITEXTFIELD_HPP

/**
 * @file ui_text_field.hpp
 * @brief Declares the retained UI text input element.
 */

#include <glm/glm.hpp>
#include "../ui_element.hpp"
#include <functional>
#include <string>

namespace CosmicEngine
{

    /**
     * @brief UI element that captures and edits user text input.
     *
     * UITextField provides a single-line editable text field with placeholder text,
     * focus management, cursor navigation, and change/submit callbacks.  Useful for
     * chat boxes, IP address entries, or any user-typed input.
     *
     * @par Example â€” creating input fields for IP and port
     * @code
     * auto* txtFieldIp = new CosmicEngine::UITextField(
     *     "127.0.0.1",       // initial text
     *     "Server IP",       // placeholder
     *     "demo_font",       // font resource key
     *     glm::vec2(centerX - 200.0f, 300.0f),  // position
     *     glm::vec2(400.0f, 45.0f),              // size
     *     64,                // max length
     *     false);            // initially hidden
     * UI_MN.AddElement(txtFieldIp);
     *
     * // Reading the value later:
     * std::string ip = txtFieldIp->GetText();
     *
     * // Toggling focus programmatically:
     * txtFieldIp->SetFocused(true);
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
        /**
         * @brief Creates a text field element.
         * @param initialText Initial text content.
         * @param placeholder Placeholder text shown when the field is empty.
         * @param font Font resource key used for rendering text.
         * @param Position Field position in UI space.
         * @param Size Field size.
         * @param maxLength Maximum allowed text length.
         * @param visible Initial visibility state.
         * @param parent Optional parent UI element.
         */
        UITextField(const std::string &initialText, const std::string &placeholder, const std::string &font,
                    glm::vec2 Position, glm::vec2 Size, int maxLength = 256, bool visible = true, UIElement *parent = nullptr);
        /** @brief Releases the text field. */
        virtual ~UITextField();

        /** @brief Updates the text field state and keyboard input handling. */
        void update(float deltaTime) override;
        /** @brief Draws the text field, its text, and its cursor state. */
        void draw() override;

        /** @brief Replaces the current text content. */
        void SetText(const std::string &text);
        /**
         * @brief Returns the current text content.
         * @return The current text content.
         */
        std::string GetText() const;

        /** @brief Sets the placeholder text shown when the field is empty. */
        void SetPlaceholder(const std::string &placeholder);
        /** @brief Sets the font resource key used by the field. */
        void SetFont(const std::string &font);
        /**
         * @brief Sets the maximum length of the field text.
         * @param maxLength Value provided by the caller.
         */
        void SetMaxLength(int maxLength);

        /**
         * @brief Sets the default text color.
         * @param color Value provided by the caller.
         */
        void SetTextColor(glm::vec3 color);
        /**
         * @brief Sets the text color used while the field is focused.
         * @param color Value provided by the caller.
         */
        void SetFocusTextColor(glm::vec3 color);
        /**
         * @brief Sets the placeholder text color.
         * @param color Value provided by the caller.
         */
        void SetPlaceholderColor(glm::vec3 color);
        /**
         * @brief Sets the border color used while the field is not focused.
         * @param color Value provided by the caller.
         */
        void SetBorderColor(glm::vec3 color);
        /**
         * @brief Sets the border color used while the field is focused.
         * @param color Value provided by the caller.
         */
        void SetFocusBorderColor(glm::vec3 color);
        /**
         * @brief Sets the background color of the field.
         * @param color Value provided by the caller.
         */
        void SetBackgroundColor(glm::vec3 color);

        /**
         * @brief Sets the focus state of the field.
         * @param focused Value provided by the caller.
         */
        void SetFocused(bool focused);
        /**
         * @brief Returns whether the field is currently focused.
         * @return The whether the field is currently focused.
         */
        bool IsFocused() const;

        /** @brief Sets the callback invoked when the field is submitted. */
        void SetOnSubmit(std::function<void(const std::string &)> callback);
        /** @brief Sets the callback invoked when the field text changes. */
        void SetOnTextChanged(std::function<void(const std::string &)> callback);
    };

}

#endif // COSMIC_UITEXTFIELD_HPP
