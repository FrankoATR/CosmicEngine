#ifndef BUTTONSLIDER_H
#define BUTTONSLIDER_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

class ButtonSlider {

private:
	int c_x, c_y, c_width, c_height;
	int s_x, s_y, s_width, s_height;
	bool inUse;
	ALLEGRO_BITMAP* sliderSprite;
	ALLEGRO_BITMAP* containerSprite;

public:

	ButtonSlider(ALLEGRO_BITMAP* containerSprite, ALLEGRO_BITMAP* sliderSprite, int c_x, int c_y, int c_width, int c_height, int s_width, int s_height, float initial_value = 0.5f);
	~ButtonSlider();
	void Draw(int);
	void Update(ALLEGRO_EVENT, float, float, int, float*);
	void SetValue(float);

};

#endif 
