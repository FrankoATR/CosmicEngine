#include "ProjectileObject.hpp"
#include "../WandAllegroEngine/Managers/BodyManager.hpp"
#include "../WandAllegroEngine/Managers/InputManager.hpp"
#include "../WandAllegroEngine/Managers/TimerManager.hpp"

#include <math.h>

ProjectileObject::ProjectileObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId, GameObject* Owner, WAND_VEC2 MoveToPosition) :
    GameObject(ObjectType, Position, Size, ObjectName, Sprite, LayerId), Owner(Owner)
{
    float dx = MoveToPosition.x - Position.x;
    float dy = MoveToPosition.y - Position.y;
    float magnitude = sqrt(dx * dx + dy * dy);
    directionX = dx / magnitude;
    directionY = dy / magnitude;
    this->Velocity = 800;
}

void ProjectileObject::Draw()
{
    al_draw_tinted_scaled_rotated_bitmap(Sprite, al_map_rgba(255, 255, 255, 255), 0, 0, Position.x, Position.y, Size.x / al_get_bitmap_width(Sprite), Size.y / al_get_bitmap_height(Sprite), 0, NULL);
}

void ProjectileObject::Init()
{
    BodyManager::GetInstance().Add(this, Position, GetSize());
    AliveTimer = new GameTimer(1.5, false, false);
    TimerManager::GetInstance().Add(AliveTimer);
}

void ProjectileObject::Update(float DeltaTime)
{
    if(!InsideGridArea)
    {
        this->Destroy();
    }
    Position.x += directionX * Velocity * DeltaTime;
    Position.y += directionY * Velocity * DeltaTime;

    if(AliveTimer && AliveTimer->IsTrigger())
    {
        this->Destroy();
    }
}

void ProjectileObject::OnCollision(GameObject* Other)
{
    if(Owner && Other != Owner)
    {
        Other->Destroy();
        this->Destroy();
    }
}

void ProjectileObject::MoveToPosition(WAND_VEC2 MoveToPosition)
{
    float dx = MoveToPosition.x - Position.x;
    float dy = MoveToPosition.y - Position.y;
    float magnitude = sqrt(dx * dx + dy * dy);
    float directionX = dx / magnitude;
    float directionY = dy / magnitude;
}