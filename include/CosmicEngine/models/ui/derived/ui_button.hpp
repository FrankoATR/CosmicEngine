#ifndef COSMIC_UIBUTTON_HPP
#define COSMIC_UIBUTTON_HPP

/**
 * @file ui_button.hpp
 * @brief Declares the retained UI button element.
 */

#include <glm/glm.hpp>
#include "../ui_element.hpp"
#include <functional>
#include <string>


namespace CosmicEngine
{

    /**
     * @brief Clickable UI element with optional text, texture, and callback behavior.
     *
     * UIButton combines a background texture, a text label, and a click callback.
     * Buttons can fire on press (default) or on release via the ReleaseMode flag.
     * Register each button with the UIManager so that it receives update and draw calls.
     *
     * @par Example â€” creating a button with click handler
     * @code
     * auto* btn = new CosmicEngine::UIButton(
     *     "Start Game",       // label text
     *     "test_font",        // font resource key
     *     "test_texture2",    // background texture key
     *     {200.0f, 0.0f},     // position
     *     {150.0f, 50.0f},    // size
     *     false,              // ReleaseMode (false = fire on press)
     *     true);              // visible
     *
     * btn->SetOnClick([this]() {
     *     std::cout << "Button clicked!" << std::endl;
     * });
     * btn->SetTextColor(glm::vec3(1.0f));
     * UI_MN.AddElement(btn);
     * @endcode
     */
    class UIButton : public UIElement
    {
    private:
        std::string texture;
        std::string text;
        std::string cachedText;
        std::string font;
        
        std::function<void()> onClick;

        glm::vec3 textColor;
        glm::vec2 cachedSize;

        float Transparency;

        bool isPressed;

        bool clickOnArea;

        bool ReleaseMode;

        void HandleInput();

    public:
        /**
         * @brief Creates a button element.
         * @param text Label rendered on the button.
         * @param font Font resource key used for the label.
         * @param texture Texture resource key used for the button background.
         * @param Position Button position in UI space.
         * @param Size Button size.
         * @param ReleaseMode True to trigger clicks on release instead of press.
         * @param visible Initial visibility state.
         * @param parent Optional parent UI element.
         */
        UIButton(const std::string &text, const std::string &font, const std::string &texture, glm::vec2 Position, glm::vec2 Size, bool ReleaseMode = false, bool visible = true, UIElement* parent = nullptr);
        /** @brief Releases the button element. */
        virtual ~UIButton();

        /** @brief Updates the button state and input handling. */
        void update(float deltaTime) override;
        /** @brief Draws the button. */
        void draw() override;

        /** @brief Sets the callback invoked when the button is clicked. */
        void SetOnClick(std::function<void()> callback);
        /** @brief Sets the button label text. */
        void SetText(const std::string &text);
        /** @brief Sets the font resource key used by the button label. */
        void SetFont(const std::string &text);

        /**
         * @brief Sets the text color of the button label.
         * @param color Value provided by the caller.
         */
        void SetTextColor(glm::vec3 color);
    };

}

#endif // COSMIC_UIBUTTON_HPP
