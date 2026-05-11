#include "GameObject.hpp"
#include "../Managers/ObjectManager.hpp"

#include <iostream>

namespace WandEngine
{

    GameObject::GameObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId) : ObjectType(ObjectType), ObjectName(ObjectName), Sprite(Sprite), Size(Size), Position(Position), LastPosition(Position), LayerId(LayerId)
    {
        this->AliveInGameManager = true;
        this->Visible = true;
        this->InsideGridArea = true;
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

    void GameObject::SetPosition(WAND_VEC2 NewPosition)
    {
        this->LastPosition = this->Position;
        this->Position = NewPosition;
    }

    WAND_VEC2 GameObject::GetPosition() const
    {
        return this->Position;
    }

    void GameObject::SetSize(WAND_VEC2 NewSize)
    {
        this->Size = NewSize;
    }

    WAND_VEC2 GameObject::GetSize() const
    {
        return this->Size;
    }

    std::string GameObject::GetObjectName() const
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

    int GameObject::GetObjectId() const
    {
        return this->ObjectId;
    }

    Object GameObject::GetObjectType() const
    {
        return this->ObjectType;
    }

    void GameObject::SetLayerId(short int NewLayerId)
    {
        this->LayerId = NewLayerId;
        WandEngine::ObjectManager::GetInstance().SortByLayer();
    }

    short int GameObject::GetLayerId() const
    {
        return this->LayerId;
    }

    void GameObject::SetSprite(ALLEGRO_BITMAP *NewSprite)
    {
        this->Sprite = NewSprite;
    }

    ALLEGRO_BITMAP *GameObject::GetSprite() const
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

    void GameObject::SetVisible(bool Visible)
    {
        this->Visible = Visible;
    }

    bool GameObject::GetVisible()
    {
        return this->Visible;
    }

    void GameObject::SetInsideGridArea(bool InsideGridArea)
    {
        this->InsideGridArea = InsideGridArea;
    }

    bool GameObject::GetInsideGridArea()
    {
        return this->InsideGridArea;
    }


    bool GameObject::GetAliveInGameManager() const
    {
        return this->AliveInGameManager;
    }

    GameObject::~GameObject()
    {
        // std::cout << "Object destroyed: " << GetObjectName() << std::endl; const
    }

}
