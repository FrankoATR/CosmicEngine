#ifndef ORB_HPP
#define ORB_HPP

#include <WandEngine/Models/GameObject.hpp>
#include <WandEngine/Models/GameBodyObject.hpp>
#include <WandEngine/Models/GameTimer.hpp>

using namespace WandEngine;

enum class OrbType
{
    Green,
    Blue,
    White,
    Orange,
    Black
};

class Orb : public GameObject
{
private:
    GameBodyObject* Body;
    OrbType Type;
    bool Used;

    GameTimer* RotateSprite_Timer;

public:
    Orb(OrbType Type, glm::vec2 Position, glm::vec2 Size, short int LayerId);
    ~Orb();
    void Init() override;
    void Draw() const override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(GameObject *other, CollisionSide Side);

    void SetUsed();
    bool IsUsed();
    OrbType GetOrbType();

    virtual Orb* Clone() const override
    {
        return new Orb(*this);
    }

    std::vector<std::string> GetAllValues() const;
    static void RegisterSerialize();
};

#endif //ORB_HPP