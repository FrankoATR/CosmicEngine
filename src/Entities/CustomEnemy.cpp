#include "CustomEnemy.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"

CustomEnemy::CustomEnemy(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font) : 
GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId), HP(HP), TimeToChangeDirection(1), TimeUntilRecibeDamage(1), ActualDirection(Direction::RIGHT)
{
    this->font = font;
    this->last_time = 0;
}


void CustomEnemy::Init(){
    BodyManager::GetInstance().Add(this, GetPosition(), GetSize());
}


void CustomEnemy::Draw(){
    GameObject::Draw();
    
    //al_draw_text(font, al_map_rgba(255,0,100, 0), GetPosition().x, GetPosition().y + 100, NULL, "test");
    //al_draw_text(font, al_map_rgba(255,200,100, 255), GetPosition().x, GetPosition().y - 60, NULL, GetObjectName().c_str());

}


void CustomEnemy::Update(float deltaTime){

    double current_time = al_get_time();
    if (current_time - last_time >= 1.0)
    {
        last_time = current_time;
        TimeToChangeDirection--;

        if (TimeToChangeDirection <= 0) {
            ActualDirection = directions[rand()%4];
            TimeToChangeDirection = 1;
        }
    }

    if(ActualDirection == Direction::UP){
        MoveUp(deltaTime);
    }
    if(ActualDirection == Direction::DOWN){
        MoveDown(deltaTime);
    }
    if(ActualDirection == Direction::LEFT){
        MoveLeft(deltaTime);
    }
    if(ActualDirection == Direction::RIGHT){
        MoveRight(deltaTime);
    }

}


void CustomEnemy::MoveUp(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x, (GetPosition().y - 200 * deltaTime)));
}

void CustomEnemy::MoveDown(float deltaTime)
{
    SetPosition(WAND_VEC2(GetPosition().x, (GetPosition().y + 200 * deltaTime)));
}


void CustomEnemy::MoveRight(float deltaTime){
    SetPosition(WAND_VEC2(GetPosition().x + 200*deltaTime, (GetPosition().y)));
}


void CustomEnemy::MoveLeft(float deltaTime){
    SetPosition(WAND_VEC2(GetPosition().x - 200*deltaTime, (GetPosition().y)));
}


void CustomEnemy::OnCollision(GameObject* other){

    if(other->GetObjectName() == "Tile"){
        other->Destroy();
    }

    if(other->GetObjectName() == "Player"){
        //other->SetSprite(this->GetSprite());
    }

    if(other->GetObjectType() == Object::DynamicEntity){

    }

}


