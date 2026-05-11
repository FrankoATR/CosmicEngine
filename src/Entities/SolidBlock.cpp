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

void SolidBlock::Draw()
{

    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", BlockID % 3, BlockID / 3, Position, Size, Rotation, MainColor, 1.0f);
}

void SolidBlock::Update(float deltaTime)
{
}

void SolidBlock::BodyCollisionEvent(GameObject *Other, CollisionSide Side)
{

}


void SolidBlock::SaveToDB()
{
    DataBaseManager::GetInstance().CreateTable("SolidBlock", "id INTEGER PRIMARY KEY AUTOINCREMENT, PositionX REAL, PositionY REAL, BlockID INTEGER");

    for(GameObject*  obj : ObjectManager::GetInstance().FindByClassName("SolidBlock"))
    {
        SolidBlock* solidblock = static_cast<SolidBlock*>(obj);
        std::ostringstream values;
        values << solidblock->GetPosition().x << ", " << solidblock->GetPosition().y << ", " << solidblock->GetBlockID();
    
        DataBaseManager::GetInstance().InsertData("SolidBlock", "PositionX, PositionY, BlockID", values.str());
    }
    
}


void SolidBlock::LoadFrom()
{
    auto callback = [](void* data, int argc, char** argv, char** colNames) -> int {
        float posX = std::stof(argv[0]);
        float posY = std::stof(argv[1]);
        int type = std::stoi(argv[2]);

        SolidBlock* block = new SolidBlock(type, glm::vec2(posX, posY), glm::vec2(100.0f), 0);
        ObjectManager::GetInstance().Add(block);

        return 0;
    };

    DataBaseManager::GetInstance().ConsultTable("SolidBlock", "PositionX, PositionY, BlockID", callback);
}


int SolidBlock::GetBlockID()
{
    return BlockID;
}