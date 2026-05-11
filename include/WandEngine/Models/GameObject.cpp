#include "GameObject.hpp"
#include "../Managers/ObjectManager.hpp"
#include "../Managers/TimerManager.hpp"
#include <math.h>

#include <iostream>

namespace WandEngine
{
    GameObject::GameObject(GameObject *Other)
    {
        ObjectType = Object::DynamicEntity;
        ObjectName = Other->GetObjectName();
        Sprite = Other->GetSprite();
        Size = Other->GetSize();
        Position = Other->GetPosition();
        LastPosition = Other->GetPosition();
        LayerId = Other->GetLayerId();
        MainColor = Other->GetColor();
        AliveInGameManager = true;
        Visible = true;
        InsideGridArea = true;
        VelocityForDuration = 0;
        MovementTimer = nullptr;
    }

    GameObject::GameObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId) : 
        ObjectType(ObjectType), 
        ObjectName(ObjectName), 
        Sprite(Sprite), 
        Size(Size), 
        Position(Position), 
        LastPosition(Position), 
        LayerId(LayerId),
        MainColor(WAND_COLOR(255, 255, 255, 255)),
        VelocityForDuration(0.0f), 
        MovementTimer(nullptr)
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
            al_draw_tinted_scaled_rotated_bitmap(Sprite, al_map_rgba(255, 255, 255, 255), 0, 0, Position.x, Position.y, Size.x / al_get_bitmap_width(Sprite), Size.y / al_get_bitmap_height(Sprite), 0, 0);
            // al_draw_bitmap(Sprite, Position.x, Position.y, NULL);
        }
    }

    void GameObject::Update(float deltaTime)
    {
        this->LastPosition = this->Position;
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

    void GameObject::UpdatePosition(float DeltaTime)
    {
        Position.x += Direction.x * Velocity.x * DeltaTime;
        Position.y += Direction.y * Velocity.y * DeltaTime;
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

    WAND_VEC2 GameObject::GetLastPosition() const
    {
        return this->LastPosition;
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

    void GameObject::SetColor(WAND_COLOR NewColor)
    {
        this->MainColor = NewColor;
    }
    
    WAND_COLOR GameObject::GetColor() const
    {
        return this->MainColor;
    }
    

    void GameObject::SetDirection(WAND_VEC2 NewDirection)
    {
        this->Direction = NewDirection;
    }

    WAND_VEC2 GameObject::GetDirection() const
    {
        return this->Direction;
    }

    void GameObject::SetRotation(short int NewRotation)
    {
        this->Rotation = NewRotation;
    }

    short int GameObject::GetRotation() const
    {
        return this->Rotation;
    }

    void GameObject::SetVelocity(WAND_VEC2 NewVelocity)
    {
        this->Velocity = NewVelocity;
    }

    WAND_VEC2 GameObject::GetVelocity() const
    {
        return this->Velocity;
    }

    bool GameObject::ReachPositionInTime(WAND_VEC2 NewPosition, double Duration, double DeltaTime)
    {
        if (VelocityForDuration == 0.0f)
        {
            float dx = NewPosition.x - Position.x;
            float dy = NewPosition.y - Position.y;
            float magnitude = sqrt(dx * dx + dy * dy);
            Direction.x = dx / magnitude;
            Direction.y = dy / magnitude;
            VelocityForDuration = magnitude / Duration;
        }
        if (fabs(NewPosition.x - Position.x) < 1 && fabs(NewPosition.y - Position.y) < 1)
        {
            VelocityForDuration = 0.0f;
            return true;
        }
        else
        {
            Position.x += Direction.x * VelocityForDuration * DeltaTime;
            Position.y += Direction.y * VelocityForDuration * DeltaTime;
            return false;
        }
    }

    bool GameObject::MoveForDirection(WAND_VEC2 NewDirection, double Duration, double DeltaTime)
    {
        if (!MovementTimer)
        {
            MovementTimer = new GameTimer(Duration, false, false);
            TimerManager::GetInstance().Add(MovementTimer);
            float dx = NewDirection.x - Position.x;
            float dy = NewDirection.y - Position.y;
            float magnitude = sqrt(dx * dx + dy * dy);
            Direction.x = dx / magnitude;
            Direction.y = dy / magnitude;
        }

        if(MovementTimer)
        {
            if (MovementTimer->IsTrigger())
            {
                MovementTimer = nullptr;
                return true;
            }
            else
            {
                UpdatePosition(DeltaTime);
                return false;
            }
        }
        else
        {
            return false;
        }
        // float radians = theta * (M_PI / 180.0f);
        // float directionX = cos(radians);
        // float directionY = sin(radians);
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
