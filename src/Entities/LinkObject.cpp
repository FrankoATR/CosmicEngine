#include "LinkObject.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"

LinkObject::LinkObject(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font) : 
GameObject(Game, ObjectType, Position, Size, ObjectName, Sprite, LayerId), HP(HP)
{
    this->font = font;
    this->DestructorMode = true;
}


void LinkObject::Init(){
    Game->gameBodyManager->Add(this, GetPosition(), GetSize());
}



void LinkObject::Draw(){
    GameObject::Draw();
    
    //al_draw_text(font, al_map_rgba(255,0,100, 0), GetPosition().x, GetPosition().y - 60, NULL, GetObjectName().c_str());

}


void LinkObject::Update(float deltaTime){

    GameObject::Update(deltaTime);

    if (Game->keyState[ALLEGRO_KEY_A]) {
        MoveLeft(deltaTime);
        EventManager::GetInstance().Notify(this, "MOVE UP");
    }
    if (Game->keyState[ALLEGRO_KEY_D]) {
        MoveRight(deltaTime);
    }
    if (Game->keyState[ALLEGRO_KEY_W]) {
        MoveUp(deltaTime);
    }
    if (Game->keyState[ALLEGRO_KEY_S]) {
        MoveDown(deltaTime);
        EventManager::GetInstance().Notify(this, "MOVE DOWN");
    }
    
    if (Game->keyState[ALLEGRO_KEY_Q]) {
        SetSize(Vec2(32,32));
    }
    if (Game->keyState[ALLEGRO_KEY_E]) {
        DestructorMode = !DestructorMode;
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
        EventManager::GetInstance().Notify(this, (this->GetObjectName() + " destroyed " + other->GetObjectName() + " D:"));
    }

    if(other->GetObjectName() == "Tile"){
        DestructorMode ? other->Destroy() : SetToLastPosition();
    }


    if(other->GetObjectType() == DynamicEntity){

    }

}


