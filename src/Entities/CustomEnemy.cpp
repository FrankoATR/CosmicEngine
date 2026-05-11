#include "CustomEnemy.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"
#include "../WandAllegroEngine/Managers/TimerManager.hpp"
#include <math.h>

CustomEnemy::CustomEnemy(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font) : 
GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId), HP(HP), CurrentDirection(Direction::RIGHT)
{
    this->font = font;
    this->Velocity = 200;
}


void CustomEnemy::Init(){
    BodyManager::GetInstance().Add(this, GetPosition(), GetSize());
    MoveTimer = new GameTimer(0.5, true, false);
    TimerManager::GetInstance().Add(MoveTimer);
}


void CustomEnemy::Draw(){
    GameObject::Draw();
    
    //al_draw_text(font, al_map_rgba(255,0,100, 0), GetPosition().x, GetPosition().y + 100, NULL, "test");
    //al_draw_text(font, al_map_rgba(255,200,100, 255), GetPosition().x, GetPosition().y - 60, NULL, GetObjectName().c_str());

}


void CustomEnemy::Update(float deltaTime){

    /*
    
    if(!GetInsideGridArea())
    {
        SetToLastPosition();
    }
    
    GameObject::Update(deltaTime);

    */

    if(MoveTimer->IsTrigger())
    {
        CurrentDirection = directions[rand()%8];
    }


    if(CurrentDirection == Direction::UP){
        MoveUp(deltaTime);
    }
    if(CurrentDirection == Direction::DOWN){
        MoveDown(deltaTime);
    }
    if(CurrentDirection == Direction::LEFT){
        MoveLeft(deltaTime);
    }
    if(CurrentDirection == Direction::RIGHT){
        MoveRight(deltaTime);
    }
    if(CurrentDirection == Direction::UP_LELFT){
        MoveUpLeft(deltaTime);
    }
    if(CurrentDirection == Direction::DOWN_LEFT){
        MoveDownLeft(deltaTime);
    }
    if(CurrentDirection == Direction::UP_RIGHT){
        MoveUpRight(deltaTime);
    }
    if(CurrentDirection == Direction::DOWM_RIGHT){
        MoveDownRight(deltaTime);
    }

}

void CustomEnemy::MoveUp(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x, (GetPosition().y - Velocity * deltaTime)));
}

void CustomEnemy::MoveDown(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x, (GetPosition().y + Velocity * deltaTime)));
}

void CustomEnemy::MoveRight(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x + Velocity * deltaTime, (GetPosition().y)));
}

void CustomEnemy::MoveLeft(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x - Velocity * deltaTime, (GetPosition().y)));
}


    
void CustomEnemy::MoveUpLeft(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x - (Velocity / sqrt(2.0f)) * deltaTime, GetPosition().y - (Velocity / sqrt(2.0f)) * deltaTime));

}

void CustomEnemy::MoveUpRight(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x + (Velocity / sqrt(2.0f)) * deltaTime, GetPosition().y - (Velocity / sqrt(2.0f)) * deltaTime));
}

void CustomEnemy::MoveDownLeft(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x - (Velocity / sqrt(2.0f)) * deltaTime, GetPosition().y + (Velocity / sqrt(2.0f)) * deltaTime));

}

void CustomEnemy::MoveDownRight(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x + (Velocity / sqrt(2.0f)) * deltaTime, GetPosition().y + (Velocity / sqrt(2.0f)) * deltaTime));
}



void CustomEnemy::OnCollision(GameObject* other){

    if(other->GetObjectName() == "Tile"){
        other->Destroy();
    }
    else
    {
        SetToLastPosition();
        CurrentDirection = directions[rand()%8];
        MoveTimer->Reset();
    }

    if(other->GetObjectType() == Object::DynamicEntity){

    }

}

CustomEnemy::~CustomEnemy()
{
    this->MoveTimer->End();
}


