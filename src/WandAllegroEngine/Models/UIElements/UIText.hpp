#ifndef UITEXT_HPP
#define UITEXT_HPP

#include "../UIElement.hpp"
#include <functional>
#include <string>
#include <iostream>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

namespace WandEngine
{

    class UIText : public UIElement
    {
    private:
        std::string text;
        ALLEGRO_FONT *font;
        WAND_COLOR textColor;

    public:
        UIText(const std::string &text, ALLEGRO_FONT *font, WAND_VEC2 Position, WAND_SIZE Size, bool visible, UIElement* parent);
        virtual ~UIText();

        void Update(float deltaTime) override;
        void Draw() override;

        void SetText(const std::string &text);
        void SetFont(ALLEGRO_FONT *font);

        void SetTextColor(WAND_COLOR color);
    };

}

#endif // UITEXT_HPP
