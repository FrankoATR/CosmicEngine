#include "ButtonSlider.h"
#include "math.h"

ButtonSlider::ButtonSlider(ALLEGRO_BITMAP* containerSprite, ALLEGRO_BITMAP* sliderSprite, int c_x, int c_y, int c_width, int c_height, int s_width, int s_height, float initial_value) :
	containerSprite(containerSprite), sliderSprite(sliderSprite), c_x(c_x), c_y(c_y), c_width(c_width), c_height(c_height), s_width(s_width), s_height(s_height) {
	//s_x = c_x + c_width / 2 - s_width / 2;
	s_x = round(pow(initial_value, 0.5) * (float)(c_width - s_width) * 100) / 100.0f + c_x;
	s_y = c_y + c_height / 2 - s_height / 2;
	inUse = false;
}

ButtonSlider::~ButtonSlider(){

}

void ButtonSlider::Draw(int relative_pos) {
	al_draw_scaled_bitmap(containerSprite, 0, 0, al_get_bitmap_width(containerSprite), al_get_bitmap_height(containerSprite), relative_pos + c_x, c_y, c_width, c_height, 0);
	al_draw_scaled_bitmap(sliderSprite, 0, 0, al_get_bitmap_width(sliderSprite), al_get_bitmap_height(sliderSprite), relative_pos + s_x, s_y, s_width, s_height, 0);

	//al_draw_rectangle(relative_pos + sliderContainerX, sliderContainerY, relative_pos + sliderContainerX + sliderContainerW, sliderContainerY + sliderContainerH, colorblanco, 2);
	//al_draw_rectangle(relative_pos + sliderX, sliderY, relative_pos + sliderX + sliderW, sliderY + sliderH, colorblanco, 2);

}

void ButtonSlider::Update(ALLEGRO_EVENT event, float proportion_W, float proportion_H, int relative_pos, float* objetive_value) {

	if (event.type == ALLEGRO_EVENT_MOUSE_AXES || event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
		
		int Mx = event.mouse.x / proportion_W;
		int My = event.mouse.y / proportion_H;

		if (Mx >= relative_pos + s_x && Mx <= relative_pos + s_x + s_width && My >= s_y && My <= s_y + s_height) {
			if (event.mouse.button & 1)
				inUse = true;
		}

		if (inUse) {
			s_x = Mx - s_width / 2 - relative_pos;

			if (s_x < c_x)
				s_x = c_x;
			if (s_x > c_x + c_width - s_width)
				s_x = c_x + c_width - s_width;

			*objetive_value = round(pow((s_x - c_x) / (float)(c_width - s_width), 2) * 100) / 100.0f;

		}
	}
	else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
		inUse = false;
	}
}

void ButtonSlider::SetValue(float value) {
	s_x = round(pow(value, 0.5) * (float)(c_width - s_width) * 100) / 100.0f + c_x;
}