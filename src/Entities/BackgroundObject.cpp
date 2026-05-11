#include "BackgroundObject.hpp"

BackgroundObject::BackgroundObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId) : 
GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId)
{

}


void BackgroundObject::Draw(){
    if(Sprite){
        al_draw_tinted_scaled_rotated_bitmap(Sprite, al_map_rgba(255,255,255, 255), 0, 0, Position.x, Position.y, Size.x/al_get_bitmap_width(Sprite), Size.y/al_get_bitmap_height(Sprite), 0, 0 );
    }
}


void BackgroundObject::Update(float deltaTime){

}

