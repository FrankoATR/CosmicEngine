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

Spike::Spike(SpikeType Type, glm::vec2 position, glm::vec2 size, short int LayerId) :
    Object("Spike", position, size, 0.0f, LayerId), Type(Type)
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
            size.x / 2 - collisionSize.x / 2,
            size.y / 2 - collisionSize.y / 2
        };
    }
    else if(Type == SpikeType::Medium)
    {
        collisionSize = {GetSize().x * 0.2f, GetSize().y * 0.2f};
        collisionPosition = {
            size.x / 2 - collisionSize.x / 2,
            size.y * 0.7
        };
    }
    else if(Type == SpikeType::Small)
    {
        collisionSize = {GetSize().x * 0.12f, GetSize().y * 0.25f};
        collisionPosition = {
            size.x / 2 - collisionSize.x / 2,
            size.y * 0.55
        };
    }
    else if(Type == SpikeType::Small)
    {

    }
    else
    {
        std::cerr << "No SpikeType found" << std::endl;
    }

    body = new Body(this, collisionPosition, collisionSize, CALLBACK_COLLISION_EVENT(BodyCollisionEvent));
    BodyManager::GetInstance().Add(body);

}


void Spike::Draw() const
{
    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", 4, static_cast<int>(Type), position, size, rotation, mainColor, 1.0f);
}


void Spike::Update(float deltaTime)
{

}


void Spike::BodyCollisionEvent(Object *Other, BodyCollisionSide Side)
{


}


SpikeType Spike::GetSpikeType()
{
    return Type;
}


std::vector<std::string> Spike::GetAllValues() const {
    return {
        std::to_string(position.x),
        std::to_string(position.y),
        std::to_string(static_cast<int>(Type)),
        std::to_string(rotation)
    };
}


void Spike::RegisterSerialize()
{
    DataBaseManager::GetInstance().RegisterSerialization(
        "Spike",
        { 
            {"PositionX", "REAL"},
            {"PositionY", "REAL"},
            {"SpikeType", "INTEGER"},
            {"Rotation", "REAL"}
        },
        [](char** argv) -> Object*
        {
            float posX = std::stof(argv[0]);
            float posY = std::stof(argv[1]);
            int type = std::stoi(argv[2]);
            float rotation = std::stof(argv[3]);
    
            Spike* spike = new Spike(static_cast<SpikeType>(type), glm::vec2(posX, posY), glm::vec2(100.0f), 0);
            spike->SetRotation(rotation);
            return spike;
        }

    );
}