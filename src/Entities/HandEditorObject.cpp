#include "HandEditorObject.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"

#include <iostream>

HandEditorObject::HandEditorObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId) : 
GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId)
{

}


void HandEditorObject::Init(){
    BodyManager::GetInstance().Add(this, GetPosition(), GetSize());
}


void HandEditorObject::Draw(){
    if(this->GetSprite()){
        //al_draw_tinted_scaled_rotated_bitmap(this->GetSprite(), al_map_rgba(255,255,255, 255), 0, 0, this->GetPosition().x, this->GetPosition().y, this->GetSize().x/al_get_bitmap_width(this->GetSprite()), this->GetSize().y/al_get_bitmap_height(this->GetSprite()), 0, NULL );
    }
    //al_draw_rectangle(this->GetPosition().x, this->GetPosition().y, )
}


void HandEditorObject::Update(float deltaTime){
    
}


void HandEditorObject::OnCollision(GameObject* other){




}




HandEditorObject::~HandEditorObject()
{

}
