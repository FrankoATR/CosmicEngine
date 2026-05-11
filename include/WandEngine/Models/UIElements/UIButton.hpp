#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include "../UIElement.hpp"
#include <functional>
#include <string>
#include <iostream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

namespace WandEngine
{

    class UIButton : public UIElement
    {
    private:
        std::string text;
        ALLEGRO_FONT *font;
        std::function<void()> onClick;

        ALLEGRO_BITMAP* Sprite;
        WAND_COLOR textColor;

        bool isPressed;

        void HandleInput();

    public:
        UIButton(ALLEGRO_BITMAP* Sprite, const std::string &text, ALLEGRO_FONT *font, WAND_VEC2 Position, WAND_SIZE Size, bool visible, UIElement* parent);
        virtual ~UIButton();

        void Update(float deltaTime) override;
        void Draw() override;

        void SetOnClick(std::function<void()> callback);
        void SetText(const std::string &text);
        void SetFont(ALLEGRO_FONT *font);

        void SetTextColor(WAND_COLOR color);
    };

}

#endif // UIBUTTON_HPP
