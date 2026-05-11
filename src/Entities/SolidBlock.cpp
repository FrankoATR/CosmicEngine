#include "SolidBlock.hpp"

#include <WandEngine/Managers/InputManager.hpp>
#include <WandEngine/Managers/TimerManager.hpp>
#include <WandEngine/Managers/ObjectManager.hpp>
#include <WandEngine/Managers/BodyManager.hpp>

SolidBlock::SolidBlock(WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId) :
GameObject(Object::StaticEntity, Position, Size, "SolidBlock", Sprite, LayerId)
{

}


void SolidBlock::Init()
{
    Body = new GameBodyObject(this, Position, GetSize(), [this](GameObject* Other, CollisionSide Side){BodyCollisionEvent(Other, Side);});
    BodyManager::GetInstance().Add(Body);
}


void SolidBlock::Draw()
{
    if(Sprite){
        al_draw_tinted_scaled_rotated_bitmap(Sprite, al_map_rgba(255, 255, 255, 255), 0, 0, Position.x, Position.y, Size.x/al_get_bitmap_width(Sprite), Size.y/al_get_bitmap_height(Sprite), 0, 0 );
    }
}


void SolidBlock::Update(float deltaTime)
{

}


void SolidBlock::BodyCollisionEvent(GameObject *Other, CollisionSide Side)
{

    if (Other->GetObjectName() == "Player")
    {



    }

}
