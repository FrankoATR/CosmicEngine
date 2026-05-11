#include "SolidBlock.hpp"

#include <WandEngine/Managers/Input/InputManager.hpp>
#include <WandEngine/Managers/Object/ObjectManager.hpp>
#include <WandEngine/Managers/Body/BodyManager.hpp>
#include <WandEngine/Managers/Camera/CameraManager.hpp>
#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/Timer/TimerManager.hpp>
#include <WandEngine/Managers/DataBase/DataBaseManager.hpp>

#include <sstream>

SolidBlock::SolidBlock(int BlockID, glm::vec2 position, glm::vec2 Size, short int LayerId) : 
    Object("SolidBlock", position, Size, 0.0f, LayerId), BlockID(BlockID)
{
}

void SolidBlock::Init()
{
    body = new Body(this, glm::vec2(0.0f), GetSize(), CALLBACK_COLLISION_EVENT(BodyCollisionEvent));
    BodyManager::GetInstance().Add(body);



}

void SolidBlock::Draw() const
{

    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", BlockID % 3, BlockID / 3, position, size, rotation, mainColor, 1.0f);
}

void SolidBlock::Update(float deltaTime)
{
}

void SolidBlock::BodyCollisionEvent(Object *Other, BodyCollisionSide Side)
{

}


int SolidBlock::GetBlockID()
{
    return BlockID;
}

std::vector<std::string> SolidBlock::GetAllValues() const {
    return {
        std::to_string(position.x),
        std::to_string(position.y),
        std::to_string(BlockID)
    };
}


void SolidBlock::RegisterSerialize()
{
    DataBaseManager::GetInstance().RegisterSerialization(
        "SolidBlock",
        { 
            {"PositionX", "REAL"},
            {"PositionY", "REAL"},
            {"BlockID" , "INTEGER"}
        },
        [](char** argv) -> Object*
        {
            float posX = std::stof(argv[0]);
            float posY = std::stof(argv[1]);
            int blockID = std::stoi(argv[2]);
        
            return new SolidBlock(blockID, glm::vec2(posX, posY), glm::vec2(100.0f), 0);
        }

    );
}