#include "Orb.hpp"
#include "Player.hpp"

#include <WandEngine/Managers/Input/InputManager.hpp>
#include <WandEngine/Managers/Object/ObjectManager.hpp>
#include <WandEngine/Managers/Body/BodyManager.hpp>
#include <WandEngine/Managers/Camera/CameraManager.hpp>
#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/Timer/TimerManager.hpp>
#include <WandEngine/Managers/DataBase/DataBaseManager.hpp>

#include <sstream>

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


void Orb::SaveToDB()
{
    DataBaseManager::GetInstance().CreateTable("Orb", "id INTEGER PRIMARY KEY AUTOINCREMENT, PositionX REAL, PositionY REAL, OrbType INTEGER");

    for(GameObject*  obj : ObjectManager::GetInstance().FindByClassName("Orb"))
    {
        Orb* orb = static_cast<Orb*>(obj);
        std::ostringstream values;
        values << orb->GetPosition().x << ", " << orb->GetPosition().y << ", " << static_cast<int>(orb->GetOrbType());
    
        DataBaseManager::GetInstance().InsertData("Orb", "PositionX, PositionY, OrbType", values.str());
    }
    
}


void Orb::LoadFrom()
{    
    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int {
        float posX = std::stof(argv[0]);
        float posY = std::stof(argv[1]);
        int type = std::stoi(argv[2]);

        Orb* block = new Orb(static_cast<OrbType>(type), glm::vec2(posX, posY), glm::vec2(100.0f), 0);
        ObjectManager::GetInstance().Add(block);

        return 0;
    };

    DataBaseManager::GetInstance().ConsultTable("Orb", "PositionX, PositionY, OrbType", callback);
}



Orb::~Orb()
{
    //RotateSprite_Timer->Destroy();
}