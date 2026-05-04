#include "ButtonText.h"
#include <iostream>

ButtonText::ButtonText(int x, int y, ALLEGRO_BITMAP* sprite, std::string text, ALLEGRO_FONT* font, ALLEGRO_DISPLAY* window) {
	this->x = x;
	this->y = y;

	int loopLong = 24;

	ALLEGRO_BITMAP* newSprite = al_create_bitmap(text.length() * loopLong + 32, 64);
	al_set_target_bitmap(newSprite);

	al_draw_bitmap_region(sprite, 0, 0, 16, 64, 0, 0, NULL);

	for (int i = 0; i < text.length(); i++)
		al_draw_bitmap_region(sprite, 16, 0, 36, 64, 16 + loopLong * i, 0, NULL);

	al_draw_bitmap_region(sprite, 48, 0, 16, 64, 16 + loopLong * text.length(), 0, NULL);

	ALLEGRO_BITMAP* newSprite2 = al_create_bitmap(al_get_bitmap_width(newSprite) * 1.6, al_get_bitmap_height(newSprite) * 1.6);
	al_set_target_bitmap(newSprite2);

	al_draw_scaled_bitmap(newSprite, 0, 0, al_get_bitmap_width(newSprite), al_get_bitmap_height(newSprite), 0, 0, al_get_bitmap_width(newSprite) * 1.6, al_get_bitmap_height(newSprite) * 1.6, 0);
	sprite_w = al_get_bitmap_width(newSprite2);
	sprite_h = al_get_bitmap_height(newSprite2);
	al_set_target_bitmap(al_get_backbuffer(window));

	this->sprite_0 = newSprite2;
	this->sprite_1 = newSprite;

	this->text = text;
	this->font = font;

	Press_Range = false;
	Press_Button = false;



}

void ButtonText::update(float relativePos) {
	this->x -= relativePos;
}

void ButtonText::updatePosition(int x, int y) {
	this->x = x;
	this->y = y;
}



ButtonText::~ButtonText() {
	al_destroy_bitmap(sprite_0);
	al_destroy_bitmap(sprite_1);
}


void ButtonText::Display(ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_EVENT Evento, float proportion_W, float proportion_H, int offSetX, int offSetY) {


	al_draw_bitmap(sprite_0, x, y, NULL);


	ALLEGRO_COLOR color = al_map_rgba(255, 255, 255, 0);

	if (Press_Range) color = al_map_rgba(255, 255, 0, 0);

	al_draw_text(font, color, x + 32 * 1.6 + offSetX, y + sprite_h / 4 - 5 + offSetY, NULL, text.c_str());

	if (Evento.type == ALLEGRO_EVENT_MOUSE_AXES || Evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
		int Mx = Evento.mouse.x / proportion_W;
		int My = Evento.mouse.y / proportion_H;
		if (Mx >= x && Mx <= x + sprite_w && My >= y && My <= y + sprite_h) {
			Press_Range = true;
			if (Evento.mouse.button & 1) Press_Button = true;
			else Press_Button = false;
		}
		else {
			Press_Range = false;
		}
	}
	else {
		Press_Button = false;
	}



}


bool ButtonText::Pressed() {
	if (Press_Button) {
		Press_Button = false;
		Press_Range = false;
		return true;
	}
	else return false;
}