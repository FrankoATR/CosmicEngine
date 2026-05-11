#include "GameObject.hpp"
#include "../Managers/ObjectManager.hpp"

#include <iostream>

GameObject::GameObject(Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId) : ObjectType(ObjectType), ObjectName(ObjectName), Sprite(Sprite), Size(Size), Position(Position), LastPosition(Position), LayerId(LayerId)
{
    this->AliveInGameManager = true;
}

void GameObject::Init()
{

}


void GameObject::Draw()
{
    if (Sprite)
    {
        // al_draw_tinted_scaled_rotated_bitmap_region(Sprite, 0, 0, 16, 16, al_map_rgba(255,255,255, 0.5), 0, 0, Position.x, Position.y, 6, 6, 0, NULL );
        al_draw_tinted_scaled_rotated_bitmap(Sprite, al_map_rgba(255, 255, 255, 255), 0, 0, Position.x, Position.y, Size.x / al_get_bitmap_width(Sprite), Size.y / al_get_bitmap_height(Sprite), 0, NULL);
        // al_draw_bitmap(Sprite, Position.x, Position.y, NULL);
    }
}

void GameObject::Update(float deltaTime)
{
    this->LastPosition = this->Position;
}

void GameObject::OnCollision(GameObject *other)
{

}

void GameObject::SetPosition(Vec2 NewPosition)
{
    this->LastPosition = this->Position;
    this->Position = NewPosition;
}

Vec2 GameObject::GetPosition()
{
    return this->Position;
}

void GameObject::SetSize(Vec2 NewSize)
{
    this->Size = NewSize;
}

Vec2 GameObject::GetSize()
{
    return this->Size;
}

std::string GameObject::GetObjectName()
{
    return this->ObjectName;
}

void GameObject::SetObjectName(std::string NewName)
{
    this->ObjectName = NewName;
}

void GameObject::SetObjectId(int NewObjectId)
{
    this->ObjectId = NewObjectId;
}

int GameObject::GetObjectId()
{
    return this->ObjectId;
}

Object GameObject::GetObjectType()
{
    return this->ObjectType;
}

void GameObject::SetLayerId(short int NewLayerId)
{
    this->LayerId = NewLayerId;
    WandEngine::ObjectManager::GetInstance().SortByLayer();
}

short int GameObject::GetLayerId()
{
    return this->LayerId;
}

void GameObject::SetSprite(ALLEGRO_BITMAP *NewSprite)
{
    this->Sprite = NewSprite;
}

ALLEGRO_BITMAP *GameObject::GetSprite()
{
    return this->Sprite;
}

void GameObject::SetToLastPosition()
{
    if (this->Position.x != this->LastPosition.x)
    {
        this->Position.x = this->LastPosition.x;
    }
    if (this->Position.y != this->LastPosition.y)
    {
        this->Position.y = this->LastPosition.y;
    }
}

void GameObject::Destroy()
{
    this->AliveInGameManager = false;
}

bool GameObject::GetAliveInGameManager()
{
    return this->AliveInGameManager;
}

GameObject::~GameObject()
{
    //std::cout << "Object destroyed: " << GetObjectName() << std::endl;
}
