#include "Orb.hpp"
#include "Player.hpp"

#include <WandEngine/Managers/InputManager.hpp>
#include <WandEngine/Managers/TimerManager.hpp>
#include <WandEngine/Managers/ObjectManager.hpp>
#include <WandEngine/Managers/BodyManager.hpp>

Orb::Orb(OrbType Type, WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId) :
GameObject(Object::StaticEntity, Position, Size, "Orb", Sprite, LayerId), Type(Type), Used(false)
{

}


void Orb::Init()
{
    Body = new GameBodyObject(this, Position, GetSize(), [this](GameObject* Other, CollisionSide Side){BodyCollisionEvent(Other, Side);});
    BodyManager::GetInstance().Add(Body);

    RotateSprite_Timer = new GameTimer(0.030, true, false);
    TimerManager::GetInstance().Add(RotateSprite_Timer);
}


void Orb::Draw()
{
    if(Sprite){
        float cx = al_get_bitmap_width(Sprite) / 2.0f;
        float cy = al_get_bitmap_height(Sprite) / 2.0f;

        float adjusted_x = Position.x + (Size.x / 2.0f);
        float adjusted_y = Position.y + (Size.y / 2.0f);
        float rad = Rotation * ALLEGRO_PI / 180.0f; 
        al_draw_tinted_scaled_rotated_bitmap(
            Sprite,
            al_map_rgba(MainColor.r, MainColor.g, MainColor.b, MainColor.a), 
            cx, cy,
            adjusted_x, adjusted_y,
            Size.x / al_get_bitmap_width(Sprite), 
            Size.y / al_get_bitmap_height(Sprite), 
            rad, 
            0
        );    
    }
}


void Orb::Update(float deltaTime)
{
    GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
    if(!player)
    {
        Used = false;
    }

    if(RotateSprite_Timer->IsTrigger())
    {
        Rotation += 10;
    }

}


void Orb::BodyCollisionEvent(GameObject *Other, CollisionSide Side)
{


}


void Orb::SetUsed()
{
    this->Used = true;
}


bool Orb::IsUsed()
{
    return this->Used;
}


OrbType Orb::GetOrbType()
{
    return this->Type;
}



Orb::~Orb()
{
    RotateSprite_Timer->End();
}