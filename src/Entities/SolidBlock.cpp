#include "SolidBlock.hpp"

#include <WandEngine/Managers/Input/InputManager.hpp>
#include <WandEngine/Managers/Object/ObjectManager.hpp>
#include <WandEngine/Managers/Body/BodyManager.hpp>
#include <WandEngine/Managers/Camera/CameraManager.hpp>
#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/Timer/TimerManager.hpp>
#include <WandEngine/Managers/DataBase/DataBaseManager.hpp>

#include <sstream>

SolidBlock::SolidBlock(int BlockID, glm::vec2 Position, glm::vec2 Size, short int LayerId) : 
    GameObject("SolidBlock", Position, Size, 0.0f, LayerId), BlockID(BlockID)
{
}

void SolidBlock::Init()
{
    Body = new GameBodyObject(this, glm::vec2(0.0f), GetSize(), [this](GameObject *Other, CollisionSide Side)
                              { BodyCollisionEvent(Other, Side); });
    BodyManager::GetInstance().Add(Body);



}

void SolidBlock::Draw() const
{

    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", BlockID % 3, BlockID / 3, Position, Size, Rotation, MainColor, 1.0f);
}

void SolidBlock::Update(float deltaTime)
{
}

void SolidBlock::BodyCollisionEvent(GameObject *Other, CollisionSide Side)
{

}


int SolidBlock::GetBlockID()
{
    return BlockID;
}

std::vector<std::string> SolidBlock::GetAllValues() const {
    return {
        std::to_string(Position.x),
        std::to_string(Position.y),
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
        [](char** argv) -> GameObject*
        {
            float posX = std::stof(argv[0]);
            float posY = std::stof(argv[1]);
            int blockID = std::stoi(argv[2]);
        
            return new SolidBlock(blockID, glm::vec2(posX, posY), glm::vec2(100.0f), 0);
        }

    );
}