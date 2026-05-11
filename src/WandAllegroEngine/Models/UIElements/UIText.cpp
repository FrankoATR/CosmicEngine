#include "UIText.hpp"
#include "../../Managers/InputManager.hpp"
#include "../../Managers/CameraManager.hpp"
#include <allegro5/allegro_font.h>

namespace WandEngine
{

    UIText::UIText(const std::string &text, ALLEGRO_FONT *font, WAND_VEC2 Position, WAND_SIZE Size, bool visible, UIElement *parent) : text(text), font(font), UIElement(Position, Size, visible, parent), textColor(WAND_COLOR(255, 255, 255))
    {
    }

    UIText::~UIText()
    {
    }

    void UIText::Update(float deltaTime)
    {
        UIElement::Update(deltaTime);
    }

    void UIText::Draw()
    {
        if (!visible)
        {
            return;
        }

        if (font)
        {
            int textWidth = al_get_text_width(font, text.c_str());
            int textHeight = al_get_font_line_height(font);

            float textX = GlobalPosition.x + (Size.width - textWidth) / 2;
            float textY = GlobalPosition.y + (Size.height - textHeight) / 2;

            al_draw_text(font, al_map_rgb(255, 255, 255), textX, textY, 0, text.c_str());
        }

        UIElement::Draw();
    }


    void UIText::SetText(const std::string &text)
    {
        this->text = text;
    }

    void UIText::SetFont(ALLEGRO_FONT *font)
    {
        this->font = font;
    }

    void UIText::SetTextColor(WAND_COLOR color)
    {
        textColor = color;
    }


}
