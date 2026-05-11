#include "Spike.hpp"

#include <WandEngine/Managers/InputManager.hpp>
#include <WandEngine/Managers/TimerManager.hpp>
#include <WandEngine/Managers/ObjectManager.hpp>
#include <WandEngine/Managers/BodyManager.hpp>

Spike::Spike(WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId) :
GameObject(Object::StaticEntity, Position, Size, "Spike", Sprite, LayerId)
{

}


void Spike::Init()
{
    WAND_VEC2 collisionSize = {GetSize().x * 0.2f, GetSize().y * 0.2f};
    WAND_VEC2 collisionPosition = {
        Position.x + (GetSize().x - collisionSize.x) / 2,
        Position.y + (GetSize().y - collisionSize.y) / 2
    };
    Body = new GameBodyObject(this, collisionPosition, collisionSize, [this](GameObject* Other, CollisionSide Side){BodyCollisionEvent(Other, Side);});
    BodyManager::GetInstance().Add(Body);
}


void Spike::Draw()
{
    if(Sprite){
        al_draw_tinted_scaled_rotated_bitmap(Sprite, al_map_rgba(255, 255, 255, 255), 0, 0, Position.x, Position.y, Size.x/al_get_bitmap_width(Sprite), Size.y/al_get_bitmap_height(Sprite), 0, 0 );
    }
}


void Spike::Update(float deltaTime)
{

}


void Spike::BodyCollisionEvent(GameObject *Other, CollisionSide Side)
{

    if (Other->GetObjectName() == "Player")
    {
        Other->Destroy();
    }

}
