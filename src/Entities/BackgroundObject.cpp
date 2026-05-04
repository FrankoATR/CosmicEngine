#include "BackgroundObject.h"

BackgroundObject::BackgroundObject(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId) : 
GameObject(Game, ObjectType, Position, Size, ObjectName, Sprite, LayerId)
{
    Game->gameObjectManager->Add(this);
}


void BackgroundObject::Draw(){
    if(this->GetSprite()){
        al_draw_tinted_scaled_rotated_bitmap(this->GetSprite(), al_map_rgba(255,255,255, 255), 0, 0, this->GetPosition().x, this->GetPosition().y, this->GetSize().x/al_get_bitmap_width(this->GetSprite()), this->GetSize().y/al_get_bitmap_height(this->GetSprite()), 0, NULL );
    }
}


void BackgroundObject::Update(float deltaTime){

}

