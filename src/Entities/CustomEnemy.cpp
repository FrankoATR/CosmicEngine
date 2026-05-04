#include "CustomEnemy.h"

CustomEnemy::CustomEnemy(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font) : 
GameObject(Game, ObjectType, Position, Size, ObjectName, Sprite, LayerId), HP(HP), TimeToChangeDirection(1.0f), TimeUntilRecibeDamage(1.0f), ActualDirection(RIGHT)
{
    this->font = font;
    Game->gameBodyManager->Add(new GameBodyObject(this, GetPosition(), GetSize()));
}


void CustomEnemy::Draw(){
    GameObject::Draw();
    
    al_draw_text(font, al_map_rgba(255,0,100, 0), GetPosition().x, GetPosition().y - 60, NULL, (std::to_string(Game->currentTime)).c_str());

}


void CustomEnemy::Update(float deltaTime){

/*
    if(Game->Event.type == ALLEGRO_EVENT_TIMER && Game->Event.timer.source == Game->FPS){

    }
*/

    if(TimeToChangeDirection >= 1.0){
        TimeToChangeDirection = 0;
        if(ActualDirection == LEFT){
            ActualDirection = RIGHT;
        }else{
            ActualDirection = LEFT;
        }
    }
    else if(TimeToChangeDirection < 0.0){
        TimeToChangeDirection = 0;
    }

    TimeToChangeDirection += 0.01;

    if(ActualDirection == LEFT){
        MoveLeft(deltaTime);
    }else{
        MoveRight(deltaTime);
    }
}


void CustomEnemy::MoveRight(float deltaTime){
    SetPosition(Vec2(GetPosition().x + 400*deltaTime, (GetPosition().y)));
}


void CustomEnemy::MoveLeft(float deltaTime){
    SetPosition(Vec2(GetPosition().x - 400*deltaTime, (GetPosition().y)));
}


void CustomEnemy::OnCollision(GameObject* other){

    if(other->GetObjectName() == "Tile"){
        other->Destroy();
    }


    if(other->GetObjectType() == DynamicEntity){

    }

}


