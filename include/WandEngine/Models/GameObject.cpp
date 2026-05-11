#include "GameObject.hpp"
#include "GameTimer.hpp"

#include "../Managers/Object/ObjectManager.hpp"
#include "../Managers/Timer/TimerManager.hpp"
#include "../Managers/DataBase/DataBaseManager.hpp"

#include <sstream>
#include <math.h>
#include <iostream>

namespace WandEngine
{
/*
    GameObject::GameObject(GameObject *Other)
    {
        ClassName = Other->GetClassName();
        Size = Other->GetSize();
        Position = Other->GetPosition();
        Rotation = Other->GetRotation();
        Velocity = glm::vec2(0.0f);
        MaxVelocity = glm::vec2(3000.0f);
        MinVelocity = glm::vec2(-3000.0f);
        AngularVelocity = 0.0f;
        Acceleration = glm::vec2(0.0f);
        LastPosition = Other->GetPosition();
        LayerId = Other->GetLayerId();
        MainColor = Other->GetColor();
        AliveInGameManager = true;
        Visible = true;
        InsideGridArea = true;
        VelocityForDuration = 0.0f;
        MovementTimer = nullptr;
    }
*/

    GameObject::GameObject(std::string ClassName, glm::vec2 Position, glm::vec2 Size, float Rotation, short int LayerId) : 
        ClassName(ClassName), 
        Size(Size), 
        Position(Position), 
        Rotation(Rotation),
        Velocity(glm::vec2(0.0f)),
        MaxVelocity(glm::vec2(3000.0f)),
        MinVelocity(glm::vec2(-3000.0f)),
        AngularVelocity(0.0f),
        Acceleration(glm::vec2(0.0f)),
        LastPosition(Position), 
        LayerId(LayerId),
        MainColor(glm::vec3(1.0f)),
        VelocityForDuration(0.0f),
        MovementTimer(nullptr),
        AliveInGameManager(true),
        Visible(true),
        InsideGridArea(true)
    {

    }

    void GameObject::Init()
    {

    }

    void GameObject::Draw()
    {

    }

    void GameObject::Update(float deltaTime)
    {

    }

    void GameObject::SetPosition(glm::vec2 NewPosition)
    {
        this->LastPosition = this->Position;
        this->Position = NewPosition;
    }

    glm::vec2 GameObject::GetPosition() const
    {
        return this->Position;
    }

    void GameObject::UpdatePosition(float DeltaTime)
    {
        Velocity += Acceleration * DeltaTime;

        Velocity = glm::clamp(Velocity, MinVelocity, MaxVelocity);

        Position += Velocity * DeltaTime;
        Rotation += AngularVelocity * DeltaTime;

        Rotation = glm::mod(Rotation, 360.0f);
        if (Rotation < 0.0f) Rotation += 360.0f;
    }


    void GameObject::SetSize(glm::vec2 NewSize)
    {
        this->Size = NewSize;
    }

    glm::vec2 GameObject::GetSize() const
    {
        return this->Size;
    }

    std::string GameObject::GetClassName() const
    {
        return this->ClassName;
    }

    void GameObject::SetClassName(std::string NewName)
    {
        this->ClassName = NewName;
    }

    void GameObject::SetID(int NewObjectId)
    {
        this->ID = NewObjectId;
    }

    int GameObject::GetID() const
    {
        return this->ID;
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


    void GameObject::UpdateLastPosition()
    {
        LastPosition = Position;
    }

    glm::vec2 GameObject::GetLastPosition() const
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

    bool GameObject::GetVisible() const
    {
        return this->Visible;
    }

    void GameObject::SetColor(glm::vec3 NewColor)
    {
        this->MainColor = NewColor;
    }
    
    glm::vec3 GameObject::GetColor() const
    {
        return this->MainColor;
    }
    
    glm::vec2 GameObject::GetDirection() const
    {
        return glm::vec2 ((glm::length(Velocity) != 0.0f) ? glm::normalize(Velocity) : glm::vec2(0.0f));
    }

    void GameObject::SetRotation(float NewRotation)
    {
        this->Rotation = NewRotation;
    }

    float GameObject::GetRotation() const
    {
        return this->Rotation;
    }

    void GameObject::SetVelocity(glm::vec2 NewVelocity)
    {
        this->Velocity = NewVelocity;
    }

    glm::vec2 GameObject::GetVelocity() const
    {
        return this->Velocity;
    }


    void GameObject::SetMaxVelocity(glm::vec2 NewMaxVelocity)
    {
        MaxVelocity = NewMaxVelocity;
    }

    glm::vec2 GameObject::GetMaxVelocity() const
    {
        return MaxVelocity;
    }


    void GameObject::SetMinVelocity(glm::vec2 NewMinVelocity)
    {
        MinVelocity = NewMinVelocity;
    }

    glm::vec2 GameObject::GetMinVelocity() const
    {
        return MinVelocity;
    }



    /*
    bool GameObject::ReachPositionInTime(glm::vec2 NewPosition, double Duration, double DeltaTime)
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

    bool GameObject::MoveForDirection(glm::vec2 NewDirection, double Duration, double DeltaTime)
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
    */


    void GameObject::SetInsideGridArea(bool InsideGridArea)
    {
        this->InsideGridArea = InsideGridArea;
    }

    bool GameObject::GetInsideGridArea() const
    {
        return this->InsideGridArea;
    }

    bool GameObject::GetAliveInGameManager() const
    {
        return this->AliveInGameManager;
    }

    GameObject* GameObject::Clone() const
    {
        return new GameObject(*this);
    }


    void GameObject::Reset()
    {

    }


    GameObject::~GameObject()
    {
        // std::cout << "Object destroyed: " << GetObjectName() << std::endl; const
    }

}
