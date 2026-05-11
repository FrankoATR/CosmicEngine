#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include <glm/glm.hpp>
#include "../UIElement.hpp"
#include <functional>
#include <string>


namespace WandEngine
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

        void Update(float deltaTime) override;
        void Draw() override;

        void SetOnClick(std::function<void()> callback);
        void SetText(const std::string &text);
        void SetFont(const std::string &text);

        void SetTextColor(glm::vec3 color);
    };

}

#endif // UIBUTTON_HPP
