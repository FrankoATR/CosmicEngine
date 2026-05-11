#include "CustomEnemy.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"
#include "../WandAllegroEngine/Managers/TimerManager.hpp"
#include "../WandAllegroEngine/Managers/EventManager.hpp"
#include "../WandAllegroEngine/Managers/SoundManager.hpp"
#include "../WandAllegroEngine/Managers/ObjectManager.hpp"

#include <math.h>

CustomEnemy::CustomEnemy(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font) : 
GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId), HP(HP), CurrentDirection(MoveDirection::RIGHT)
{
    this->font = font;
    this->Velocity = 200;
    InitFollow = false;
}


void CustomEnemy::Init(){
    BodyManager::GetInstance().Add(this, Position, GetSize());
    //MoveTimer = new GameTimer(0.5, true, false);
    //TimerManager::GetInstance().Add(MoveTimer);
}


void CustomEnemy::Draw(){
    GameObject::Draw();
    
    //al_draw_text(font, al_map_rgba(255,0,100, 0), Position.x, Position.y + 100, NULL, "test");
    //al_draw_text(font, al_map_rgba(255,200,100, 255), Position.x, Position.y - 60, NULL, GetObjectName().c_str());

}


void CustomEnemy::Update(float deltaTime){

    LastPosition = Position;

    /*
        if(MoveTimer->IsTrigger())
    {
        CurrentDirection = directions[rand()%8];
    }
    */

    GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
    if (player)
    {
        MoveForDirection(player->GetPosition(), 0.2, deltaTime);
    }

/*

    else
    {
        if(CurrentDirection == MoveDirection::UP){
            MoveUp(deltaTime);
        }
        if(CurrentDirection == MoveDirection::DOWN){
            MoveDown(deltaTime);
        }
        if(CurrentDirection == MoveDirection::LEFT){
            MoveLeft(deltaTime);
        }
        if(CurrentDirection == MoveDirection::RIGHT){
            MoveRight(deltaTime);
        }
        if(CurrentDirection == MoveDirection::UP_LELFT){
            MoveUpLeft(deltaTime);
        }
        if(CurrentDirection == MoveDirection::DOWN_LEFT){
            MoveDownLeft(deltaTime);
        }
        if(CurrentDirection == MoveDirection::UP_RIGHT){
            MoveUpRight(deltaTime);
        }
        if(CurrentDirection == MoveDirection::DOWM_RIGHT){
            MoveDownRight(deltaTime);
        }
    }

*/

}

void CustomEnemy::MoveUp(float deltaTime)
{
    SetPosition(WAND_VEC2(Position.x, (Position.y - Velocity * deltaTime)));
}

void CustomEnemy::MoveDown(float deltaTime)
{
    SetPosition(WAND_VEC2(Position.x, (Position.y + Velocity * deltaTime)));
}

void CustomEnemy::MoveRight(float deltaTime)
{
    SetPosition(WAND_VEC2(Position.x + Velocity * deltaTime, (Position.y)));
}

void CustomEnemy::MoveLeft(float deltaTime)
{
    SetPosition(WAND_VEC2(Position.x - Velocity * deltaTime, (Position.y)));
}


    
void CustomEnemy::MoveUpLeft(float deltaTime)
{
    SetPosition(WAND_VEC2(Position.x - (Velocity / sqrt(2.0f)) * deltaTime, Position.y - (Velocity / sqrt(2.0f)) * deltaTime));

}

void CustomEnemy::MoveUpRight(float deltaTime)
{
    SetPosition(WAND_VEC2(Position.x + (Velocity / sqrt(2.0f)) * deltaTime, Position.y - (Velocity / sqrt(2.0f)) * deltaTime));
}

void CustomEnemy::MoveDownLeft(float deltaTime)
{
    SetPosition(WAND_VEC2(Position.x - (Velocity / sqrt(2.0f)) * deltaTime, Position.y + (Velocity / sqrt(2.0f)) * deltaTime));

}

void CustomEnemy::MoveDownRight(float deltaTime)
{
    SetPosition(WAND_VEC2(Position.x + (Velocity / sqrt(2.0f)) * deltaTime, Position.y + (Velocity / sqrt(2.0f)) * deltaTime));
}



void CustomEnemy::OnCollision(GameObject* other){

    if(other->GetObjectName() == "Tile"){
        //other->Destroy();
        SetToLastPosition();

        Direction.x *= -1;
        Direction.y *= -1;

        CurrentDirection = directions[rand()%8];
        //MoveTimer->Reset();
    }

    if(other->GetObjectName() == "Enemy"){
        SetToLastPosition();
        Direction.x *= -1;
        Direction.y *= -1;
        CurrentDirection = directions[rand()%8];
        //MoveTimer->Reset();
    }

    if(other->GetObjectName() == "Player")
    {
        other->Destroy();
    }

    if(other->GetObjectType() == Object::DynamicEntity){

    }

}

CustomEnemy::~CustomEnemy()
{
    SoundManager::GetInstance().Play("EnemyDeath", 1.0f, false);
    EventManager::GetInstance().TriggerEvent("OnEnemyDestroy");
    //this->MoveTimer->End();
}


