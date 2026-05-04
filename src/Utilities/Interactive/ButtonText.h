#pragma once
#ifndef BUTTONTEXT_H
#define BUTTONTEXT_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string>

class ButtonText {

public:

	int x, y;
	int sprite_w, sprite_h;
	ALLEGRO_BITMAP* sprite_0;
	ALLEGRO_BITMAP* sprite_1;
	std::string text;
	ALLEGRO_FONT* font;

	//float proportion_W;
	//float proportion_H;
	bool Press_Range;
	bool Press_Button;

	ButtonText(int, int, int, int, ALLEGRO_BITMAP*, ALLEGRO_BITMAP*);
	ButtonText(int x, int y, ALLEGRO_BITMAP* sprite, std::string text, ALLEGRO_FONT* font, ALLEGRO_DISPLAY* window);
	~ButtonText();
	void Display(ALLEGRO_EVENT_QUEUE*, ALLEGRO_EVENT, float, float, int offSetX = 0, int offSetY = 0);
	bool Pressed();
	void update(float);
	void updatePosition(int x, int y);

};

#endif 
