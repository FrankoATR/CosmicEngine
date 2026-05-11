#include "Ground.hpp"

#include <WandEngine/Managers/InputManager.hpp>
#include <WandEngine/Managers/TimerManager.hpp>
#include <WandEngine/Managers/ObjectManager.hpp>
#include <WandEngine/Managers/BodyManager.hpp>

Ground::Ground(WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId) :
GameObject(Object::StaticEntity, Position, Size, "Ground", Sprite, LayerId)
{

}


void Ground::Init()
{
    Body = new GameBodyObject(this, Position, GetSize(), [this](GameObject* Other, CollisionSide Side){BodyCollisionEvent(Other, Side);});
    BodyManager::GetInstance().Add(Body);
}


void Ground::Draw()

{
    if(Sprite){
        al_draw_tinted_scaled_rotated_bitmap(Sprite, al_map_rgba(MainColor.r, MainColor.g, MainColor.b, MainColor.a), 0, 0, Position.x, Position.y, Size.x/al_get_bitmap_width(Sprite), Size.y/al_get_bitmap_height(Sprite), 0, 0 );
    }
}


void Ground::Update(float deltaTime)
{

}


void Ground::BodyCollisionEvent(GameObject *Other, CollisionSide Side)
{

    if (Other->GetObjectName() == "Player")
    {
        //Other->SetPosition(WAND_VEC2(Other->GetPosition().x, Position.y - Other->GetSize().y));
        //Other->SetDirection(WAND_VEC2(Other->GetDirection().x, 0));
    }

}
