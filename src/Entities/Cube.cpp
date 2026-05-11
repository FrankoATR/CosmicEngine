#include "Cube.hpp"

#include <WandEngine/Managers/Input/InputManager.hpp>
#include <WandEngine/Managers/Object/ObjectManager.hpp>
#include <WandEngine/Managers/Body/BodyManager.hpp>
#include <WandEngine/Managers/Camera/CameraManager.hpp>
#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/Timer/TimerManager.hpp>
#include <WandEngine/Managers/DataBase/DataBaseManager.hpp>

#include <sstream>
#include <iostream>

Cube::Cube(glm::vec3 position, glm::vec3 size) :
    Object("Cube", position, size, glm::vec3(0.0f))
{

}


void Cube::Init()
{
    glm::vec3 collisionSize;
    glm::vec3 collisionPosition;

/*

    collisionSize = {size.x * 0.12f, size.y * 0.25f};
    collisionPosition = {
        size.x / 2 - collisionSize.x / 2,
        size.y * 0.55
    };


    body = new Body(this, collisionPosition, collisionSize, nullptr);
    BodyManager::GetInstance().Add(body);
*/

}


void Cube::Draw() const
{
    //ResourceManager::GetInstance().RenderCubeLines(position, size, position + size/2.0f, rotation, glm::vec3(1.0f, 1.0f, 0.0f), 1.0f, 1.0f);
    ResourceManager::GetInstance().RenderParallelepipedTexture("cobblestone", position, size, position + size/2.0f, rotation, glm::vec3(1.0f, 1.0f, 0.0f));
}


void Cube::Update(float deltaTime)
{

}


void Cube::BodyCollisionEvent(Object *Other, BodyCollisionSide Side)
{


}




std::vector<std::string> Cube::GetAllValues() const {
    return {
        std::to_string(position.x),
        std::to_string(position.y),
        std::to_string(position.z)
    };
}


void Cube::RegisterSerialize()
{
    DataBaseManager::GetInstance().RegisterSerialization(
        "Cube",
        { 
            {"PositionX", "REAL"},
            {"PositionY", "REAL"},
            {"PositionZ", "REAL"},
        },
        [](char** argv) -> Object*
        {
            float posX = std::stof(argv[0]);
            float posY = std::stof(argv[1]);
            float posZ = std::stof(argv[3]);

            return new Cube(glm::vec3(posX, posY, posZ), glm::vec3(1.0f));
        }

    );
}