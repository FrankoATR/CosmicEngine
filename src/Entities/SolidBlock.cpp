#include "SolidBlock.hpp"

#include <WandEngine/Managers/Input/InputManager.hpp>
#include <WandEngine/Managers/Object/ObjectManager.hpp>
#include <WandEngine/Managers/Body/BodyManager.hpp>
#include <WandEngine/Managers/Camera/CameraManager.hpp>
#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/Timer/TimerManager.hpp>

#include <random>

SolidBlock::SolidBlock(glm::vec2 Position, glm::vec2 Size, short int LayerId) : GameObject("SolidBlock", Position, Size, 0.0f, LayerId)
{
}

void SolidBlock::Init()
{
    Body = new GameBodyObject(this, glm::vec2(0.0f), GetSize(), [this](GameObject *Other, CollisionSide Side)
                              { BodyCollisionEvent(Other, Side); });
    BodyManager::GetInstance().Add(Body);

    std::random_device rd;

    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 2);

    randomNumber1 = dist(gen);
    randomNumber2 = dist(gen);
}

void SolidBlock::Draw()
{

    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", randomNumber1, randomNumber2, Position, Size, Rotation, MainColor, 1.0f);
}

void SolidBlock::Update(float deltaTime)
{
}

void SolidBlock::BodyCollisionEvent(GameObject *Other, CollisionSide Side)
{
}
