#ifndef ORB_HPP
#define ORB_HPP

#include <WandEngine/Models/GameObject.hpp>
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
    Orb(OrbType Type, WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId);
    ~Orb();
    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(GameObject *other, CollisionSide Side);

    void SetUsed();
    bool IsUsed();
    OrbType GetOrbType();
};

#endif //ORB_HPP