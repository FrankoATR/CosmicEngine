#include "EndLevel.hpp"

#include <WandEngine/Managers/InputManager.hpp>
#include <WandEngine/Managers/TimerManager.hpp>
#include <WandEngine/Managers/ObjectManager.hpp>
#include <WandEngine/Managers/BodyManager.hpp>
#include <WandEngine/Managers/SoundManager.hpp>

EndLevel::EndLevel(WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId) :
GameObject(Object::StaticEntity, Position, Size, "EndLevel", Sprite, LayerId)
{

}


void EndLevel::Init()
{
    Body = new GameBodyObject(this, Position, GetSize(), [this](GameObject* Other, CollisionSide Side){BodyCollisionEvent(Other, Side);});
    BodyManager::GetInstance().Add(Body);
}


void EndLevel::Draw()
{
    if(Sprite){
        al_draw_tinted_scaled_rotated_bitmap(Sprite, al_map_rgba(255, 255, 255, 255), 0, 0, Position.x, Position.y, Size.x/al_get_bitmap_width(Sprite), Size.y/al_get_bitmap_height(Sprite), 0, 0 );
    }
}


void EndLevel::Update(float deltaTime)
{

}


void EndLevel::BodyCollisionEvent(GameObject *Other, CollisionSide Side)
{

    if (Other->GetObjectName() == "Player")
    {
        SoundManager::GetInstance().Play("EndLevel", 1.0, false);
        Destroy();
        Other->Destroy();
        Body->Destroy();
    }

}


EndLevel::~EndLevel()
{
}