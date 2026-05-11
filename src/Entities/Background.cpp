#include "Background.hpp"

#include <WandEngine/Managers/ObjectManager.hpp>

Background::Background(WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId) :
GameObject(Object::StaticEntity, Position, Size, "Background", Sprite, LayerId)
{
    Velocity = 400;
    Direction.x = 2.3;
}


void Background::Init()
{


}


void Background::Draw()
{
    if(Sprite){
        al_draw_tinted_scaled_rotated_bitmap(Sprite, al_map_rgba(MainColor.r, MainColor.g, MainColor.b, MainColor.a), 0, 0, Position.x, Position.y, Size.x/al_get_bitmap_width(Sprite), Size.y/al_get_bitmap_height(Sprite), 0, 0 );
    }
}


void Background::Update(float deltaTime)
{
    GameObject* player = ObjectManager::GetInstance().FindByUniqueName("Player");
    if(!player)
    {
        Direction.x = 0;
    }

    UpdatePosition(Velocity, deltaTime);

}