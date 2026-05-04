#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string>

class Button {

public:

	int x, y;
	int sprite_w, sprite_h;
	ALLEGRO_BITMAP* sprite_0;
	ALLEGRO_BITMAP* sprite_1;

	//float proportion_W;
	//float proportion_H;
	bool Press_Range;
	bool Press_Button;

	Button(int, int, int, int, ALLEGRO_BITMAP*, ALLEGRO_BITMAP*);
	~Button();
	void Display(ALLEGRO_EVENT_QUEUE*, ALLEGRO_EVENT, float, float);
	bool Pressed();
	void update(float);
	void updatePosition(int x, int y);

};

#endif 
