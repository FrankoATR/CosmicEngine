#ifndef COSMIC_UITEXT_HPP
#define COSMIC_UITEXT_HPP

/**
 * @file ui_text.hpp
 * @brief Declares the retained UI text label element.
 */

#include <glm/glm.hpp>
#include "../ui_element.hpp"
#include <functional>
#include <string>


namespace CosmicEngine
{

    /**
     * @brief UI element that renders a text label.
     *
     * UIText displays a string using a loaded font resource.  Use it for titles,
     * HUD counters, or any static/dynamic text overlay.  Text color and content
     * can be changed at runtime with SetText() and SetTextColor().
     *
     * @par Example â€” creating and updating a HUD label
     * @code
     * auto* title = new CosmicEngine::UIText(
     *     "Score: 0",        // initial text
     *     "test_font",       // font resource key
     *     glm::vec2(0.0f, 24.0f),     // position
     *     glm::vec2(1920.0f, 36.0f),  // bounds
     *     true);                       // visible
     *
     * title->SetTextColor(glm::vec3(0.55f, 0.85f, 0.95f));
     * UI_MN.AddElement(title);
     *
     * // Later, update the text:
     * title->SetText("Score: " + std::to_string(score));
     * @endcode
     */
    class UIText : public UIElement
    {
    private:
        std::string text;
        std::string cachedText;
        std::string font;

        glm::vec2 cachedSize;

        glm::vec3 textColor;
        float Transparency;

    public:
        /**
         * @brief Creates a text label element.
         * @param text Initial text content.
         * @param font Font resource key used for rendering.
         * @param Position Label position in UI space.
         * @param Size Label bounds.
         * @param visible Initial visibility state.
         * @param parent Optional parent UI element.
         */
        UIText(const std::string &text, const std::string &font, glm::vec2 Position, glm::vec2 Size, bool visible = true, UIElement* parent = nullptr);
        /** @brief Releases the text element. */
        virtual ~UIText();

        /** @brief Updates cached layout state for the text element. */
        void update(float deltaTime) override;
        /** @brief Draws the text element. */
        void draw() override;

        /** @brief Sets the displayed text. */
        void SetText(const std::string &text);
        /** @brief Sets the font resource key used by the label. */
        void SetFont(const std::string &font);

        /**
         * @brief Sets the text color.
         * @param color Value provided by the caller.
         */
        void SetTextColor(glm::vec3 color);
    };

}

#endif // COSMIC_UITEXT_HPP
