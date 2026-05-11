#include "Spike.hpp"


#include <WandEngine/Managers/Input/InputManager.hpp>
#include <WandEngine/Managers/Object/ObjectManager.hpp>
#include <WandEngine/Managers/Body/BodyManager.hpp>
#include <WandEngine/Managers/Camera/CameraManager.hpp>
#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/Timer/TimerManager.hpp>

#include <random>


Spike::Spike(glm::vec2 Position, glm::vec2 Size, short int LayerId) :
GameObject("Spike", Position, Size, 0.0f, LayerId)
{

}


void Spike::Init()
{
    glm::vec2 collisionSize = {GetSize().x * 0.2f, GetSize().y * 0.4f};
    glm::vec2 collisionPosition = {
        (GetSize().x - collisionSize.x) / 2,
        (GetSize().y - collisionSize.y) / 2
    };
    Body = new GameBodyObject(this, collisionPosition, collisionSize, [this](GameObject* Other, CollisionSide Side){BodyCollisionEvent(Other, Side);});
    BodyManager::GetInstance().Add(Body);


    std::random_device rd;

    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 2);

    randomNumber1 = dist(gen);
}


void Spike::Draw()
{
    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", 4, randomNumber1, Position, Size, Rotation, MainColor, 1.0f);
}


void Spike::Update(float deltaTime)
{

}


void Spike::BodyCollisionEvent(GameObject *Other, CollisionSide Side)
{


}
