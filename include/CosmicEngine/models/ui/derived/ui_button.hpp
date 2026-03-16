#ifndef COSMIC_UIBUTTON_HPP
#define COSMIC_UIBUTTON_HPP

#include <glm/glm.hpp>
#include "../ui_element.hpp"
#include <functional>
#include <string>


namespace CosmicEngine
{

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
        UIButton(const std::string &text, const std::string &font, const std::string &texture, glm::vec2 Position, glm::vec2 Size, bool ReleaseMode = false, bool visible = true, UIElement* parent = nullptr);
        virtual ~UIButton();

        void update(float deltaTime) override;
        void draw() override;

        void SetOnClick(std::function<void()> callback);
        void SetText(const std::string &text);
        void SetFont(const std::string &text);

        void SetTextColor(glm::vec3 color);
    };

}

#endif // COSMIC_UIBUTTON_HPP
