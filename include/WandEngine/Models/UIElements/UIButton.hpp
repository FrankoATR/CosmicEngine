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
        std::string text;
        int *font;
        std::function<void()> onClick;

        unsigned int* Sprite;
        glm::vec3 textColor;
        float Transparency;

        bool isPressed;

        void HandleInput();

    public:
        UIButton(unsigned int* Sprite, const std::string &text, int *font, glm::vec2 Position, glm::vec2 Size, bool visible, UIElement* parent);
        virtual ~UIButton();

        void Update(float deltaTime) override;
        void Draw() override;

        void SetOnClick(std::function<void()> callback);
        void SetText(const std::string &text);
        void SetFont(int *font);

        void SetTextColor(glm::vec3 color);
    };

}

#endif // UIBUTTON_HPP
