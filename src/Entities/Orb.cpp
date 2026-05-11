#include "Orb.hpp"
#include "Player.hpp"

#include <WandEngine/Managers/Object/ObjectManager.hpp>
#include <WandEngine/Managers/Body/BodyManager.hpp>
#include <WandEngine/Managers/Resource/ResourceManager.hpp>
#include <WandEngine/Managers/DataBase/DataBaseManager.hpp>

#include <sstream>

Orb::Orb(OrbType Type, glm::vec2 Position, glm::vec2 Size, short int LayerId) :
    Object("Orb", Position, Size, 0.0f, LayerId), Type(Type), Used(false)
{
    angularVelocity = 500.0f;
}


void Orb::Init()
{
    glm::vec2 collisionSize = {size.x * 1.2f, size.y * 1.2f};
    glm::vec2 collisionPosition = {
        (size.x - collisionSize.x) / 2,
        (size.y - collisionSize.y) / 2
    };

    body = new Body(this, collisionPosition, collisionSize, CALLBACK_COLLISION_EVENT(BodyCollisionEvent)); // TODO: increse hitbox
    BodyManager::GetInstance().Add(body);
}


void Orb::Draw() const
{
    int idx = Type == OrbType::Green ? 0 : 1;

    ResourceManager::GetInstance().Render2DSpriteFromTextureSheet("gd", 5, idx, position, size, rotation, mainColor, 1.0f);
}


void Orb::Update(float deltaTime)
{

}


void Orb::BodyCollisionEvent(Object *Other, BodyCollisionSide Side)
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


std::vector<std::string> Orb::GetAllValues() const {
    return {
        std::to_string(position.x),
        std::to_string(position.y),
        std::to_string(static_cast<int>(Type))
    };
}


void Orb::RegisterSerialize()
{
    DataBaseManager::GetInstance().RegisterSerialization(
        "Orb",
        { 
            {"PositionX", "REAL"},
            {"PositionY", "REAL"},
            {"OrbType", "INTEGER"}
        },
        [](char** argv) -> Object*
        {
            float posX = std::stof(argv[0]);
            float posY = std::stof(argv[1]);
            int type = std::stoi(argv[2]);
    
            return new Orb(static_cast<OrbType>(type), glm::vec2(posX, posY), glm::vec2(100.0f), 0);
        }

    );
}