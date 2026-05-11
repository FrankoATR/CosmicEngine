#include "Spike.hpp"


#include <WandEngine/Managers/Input/InputManager.hpp>
#include <WandEngine/Managers/Object/ObjectManager.hpp>
#include <WandEngine/Managers/Body/BodyManager.hpp>
#include <WandEngine/Managers/Camera/CameraManager.hpp>
#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/Timer/TimerManager.hpp>
#include <WandEngine/Managers/DataBase/DataBaseManager.hpp>

#include <sstream>
#include <iostream>

Spike::Spike(SpikeType Type, glm::vec2 Position, glm::vec2 Size, short int LayerId) :
GameObject("Spike", Position, Size, 0.0f, LayerId), Type(Type)
{

}


void Spike::Init()
{
    glm::vec2 collisionSize;
    glm::vec2 collisionPosition;

    if(Type == SpikeType::Big)
    {
        collisionSize = {GetSize().x * 0.2f, GetSize().y * 0.4f};
        collisionPosition = {
            Size.x / 2 - collisionSize.x / 2,
            Size.y / 2 - collisionSize.y / 2
        };
    }
    else if(Type == SpikeType::Medium)
    {
        collisionSize = {GetSize().x * 0.2f, GetSize().y * 0.2f};
        collisionPosition = {
            Size.x / 2 - collisionSize.x / 2,
            Size.y * 0.7
        };
    }
    else if(Type == SpikeType::Small)
    {
        collisionSize = {GetSize().x * 0.12f, GetSize().y * 0.25f};
        collisionPosition = {
            Size.x / 2 - collisionSize.x / 2,
            Size.y * 0.55
        };
    }
    else if(Type == SpikeType::Small)
    {

    }
    else
    {
        std::cerr << "No SpikeType found" << std::endl;
    }

    Body = new GameBodyObject(this, collisionPosition, collisionSize, [this](GameObject* Other, CollisionSide Side){BodyCollisionEvent(Other, Side);});
    BodyManager::GetInstance().Add(Body);

}


void Spike::Draw()
{
    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", 4, static_cast<int>(Type), Position, Size, Rotation, MainColor, 1.0f);
}


void Spike::Update(float deltaTime)
{

}


void Spike::BodyCollisionEvent(GameObject *Other, CollisionSide Side)
{


}



void Spike::SaveToDB()
{
    DataBaseManager::GetInstance().CreateTable("Spike", "id INTEGER PRIMARY KEY AUTOINCREMENT, PositionX REAL, PositionY REAL, SpikeType INTEGER, Rotation REAL");

    for(GameObject* obj : ObjectManager::GetInstance().FindByClassName("Spike"))
    {
        Spike* spike = static_cast<Spike*>(obj);
        std::ostringstream values;
        values << spike->GetPosition().x << ", " << spike->GetPosition().y << ", " << static_cast<int>(spike->GetSpikeType()) << ", " << spike->GetRotation();
    
        DataBaseManager::GetInstance().InsertData("Spike", "PositionX, PositionY, SpikeType, Rotation", values.str());
    }
    
}


void Spike::LoadFrom()
{
    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int {
        float posX = std::stof(argv[0]);
        float posY = std::stof(argv[1]);
        int type = std::stoi(argv[2]);
        float rotation = std::stof(argv[3]);

        Spike* spike = new Spike(static_cast<SpikeType>(type), glm::vec2(posX, posY), glm::vec2(100.0f), 0);
        spike->SetRotation(rotation);
        ObjectManager::GetInstance().Add(spike);

        return 0;
    };

    DataBaseManager::GetInstance().ConsultTable("Spike", "PositionX, PositionY, SpikeType, Rotation", callback);
}


SpikeType Spike::GetSpikeType()
{
    return Type;
}