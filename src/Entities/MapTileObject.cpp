#include "MapTileObject.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"

#include <iostream>

MapTileObject::MapTileObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId) : 
GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId)
{

}


void MapTileObject::Init(){
    BodyManager::GetInstance().Add(this, GetPosition(), GetSize());
}


void MapTileObject::Draw(){
    if(this->GetSprite()){
        al_draw_tinted_scaled_rotated_bitmap(this->GetSprite(), al_map_rgba(255,255,255, 255), 0, 0, this->GetPosition().x, this->GetPosition().y, this->GetSize().x/al_get_bitmap_width(this->GetSprite()), this->GetSize().y/al_get_bitmap_height(this->GetSprite()), 0, 0 );
    }
}


void MapTileObject::Update(float deltaTime){
   // ReachPositionInTime(WAND_VEC2(0,0), 10.0, deltaTime);
}


void MapTileObject::OnCollision(GameObject* other){

    if(other->GetObjectName() == "Player"){
        
    }


}




MapTileObject::~MapTileObject()
{
}
