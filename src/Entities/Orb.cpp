#include "Orb.hpp"
#include "Player.hpp"

#include <WandEngine/Managers/Input/InputManager.hpp>
#include <WandEngine/Managers/Object/ObjectManager.hpp>
#include <WandEngine/Managers/Body/BodyManager.hpp>
#include <WandEngine/Managers/Camera/CameraManager.hpp>
#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/Timer/TimerManager.hpp>

Orb::Orb(OrbType Type, glm::vec2 Position, glm::vec2 Size, short int LayerId) :
GameObject("Orb", Position, Size, 0.0f, LayerId), Type(Type), Used(false)
{
    AngularVelocity = 500.0f;
}


void Orb::Init()
{
    Body = new GameBodyObject(this, glm::vec2(0.0f), GetSize(), [this](GameObject* Other, CollisionSide Side){BodyCollisionEvent(Other, Side);});
    BodyManager::GetInstance().Add(Body);

    //RotateSprite_Timer = new GameTimer(0.030, true, false);
    //TimerManager::GetInstance().Add(RotateSprite_Timer);
}


void Orb::Draw()
{
    int idx = Type == OrbType::Green ? 0 : 1;

    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", 5, idx, Position, Size, Rotation, MainColor, 1.0f);
}


void Orb::Update(float deltaTime)
{

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
    //RotateSprite_Timer->Destroy();
}