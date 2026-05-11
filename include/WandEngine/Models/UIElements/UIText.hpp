#ifndef UITEXT_HPP
#define UITEXT_HPP

#include <glm/glm.hpp>
#include "../UIElement.hpp"
#include <functional>
#include <string>


namespace WandEngine
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

        void Update(float deltaTime) override;
        void Draw() override;

        void SetText(const std::string &text);
        void SetFont(const std::string &font);

        void SetTextColor(glm::vec3 color);
    };

}

#endif // UITEXT_HPP
