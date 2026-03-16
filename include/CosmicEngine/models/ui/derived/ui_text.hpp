#ifndef COSMIC_UITEXT_HPP
#define COSMIC_UITEXT_HPP

#include <glm/glm.hpp>
#include "../ui_element.hpp"
#include <functional>
#include <string>


namespace CosmicEngine
{

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
        UIText(const std::string &text, const std::string &font, glm::vec2 Position, glm::vec2 Size, bool visible = true, UIElement* parent = nullptr);
        virtual ~UIText();

        void update(float deltaTime) override;
        void draw() override;

        void SetText(const std::string &text);
        void SetFont(const std::string &font);

        void SetTextColor(glm::vec3 color);
    };

}

#endif // COSMIC_UITEXT_HPP
