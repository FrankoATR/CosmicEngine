#include "LinkObject.h"

LinkObject::LinkObject(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font) : 
GameObject(Game, ObjectType, Position, Size, ObjectName, Sprite, LayerId), HP(HP)
{
    this->font = font;
    Game->gameBodyManager->Add(new GameBodyObject(this, GetPosition(), GetSize()));
}


void LinkObject::Draw(){
    GameObject::Draw();
    
    //al_draw_text(font, al_map_rgba(255,0,100, 0), GetPosition().x, GetPosition().y - 60, NULL, GetObjectName().c_str());

}


void LinkObject::Update(float deltaTime){

    if (Game->keyState[ALLEGRO_KEY_A]) {
        MoveLeft(deltaTime);
    }
    if (Game->keyState[ALLEGRO_KEY_D]) {
        MoveRight(deltaTime);
    }
    if (Game->keyState[ALLEGRO_KEY_W]) {
        MoveUp(deltaTime);
    }
    if (Game->keyState[ALLEGRO_KEY_S]) {
        MoveDown(deltaTime);
    }
}

void LinkObject::MoveUp(float deltaTime){
    SetPosition(Vec2(GetPosition().x, (GetPosition().y - 400*deltaTime)));
}


void LinkObject::MoveDown(float deltaTime){
    SetPosition(Vec2(GetPosition().x, (GetPosition().y + 400*deltaTime)));
}


void LinkObject::MoveRight(float deltaTime){
    SetPosition(Vec2(GetPosition().x + 400*deltaTime, (GetPosition().y)));
}


void LinkObject::MoveLeft(float deltaTime){
    SetPosition(Vec2(GetPosition().x - 400*deltaTime, (GetPosition().y)));
}


void LinkObject::OnCollision(GameObject* other){

    if(other->GetObjectName() == "Enemy"){
        other->Destroy();
    }

    if(other->GetObjectName() == "Tile"){
        other->Destroy();
    }


    if(other->GetObjectType() == DynamicEntity){

    }

}


