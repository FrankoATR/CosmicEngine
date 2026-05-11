#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <WandEngine/Models/Object/Object.hpp>
#include <WandEngine/Models/Body/Body.hpp>
#include <WandEngine/Models/Timer/Timer.hpp>
#include <random>

using namespace WandEngine;

enum class PlayerMode
{
    Normal,
    Mini,
    Ship,
    Ball,
    Ufo,
    Wave,
    Swim
};

class Player : public Object
{
private:
    std::string UniqueName;

    bool OnGroundOrBlock;
    float Gravity;

    Body* body1;
    Body* body2;

    bool KeySpaceDown, KeySpaceRelease;
    bool RigthClickDown, RigthClickRelease;
    bool ReleaseJumping;

    PlayerMode CurrentPlayerMode;

    std::random_device rd;

    Timer* TimeToEndTimer;

    int randomNumber1;

public:
    Player(std::string UniqueName, PlayerMode mode, glm::vec2 Position, glm::vec2 Size, float Rotation, short int LayerId);
    ~Player();

    void Init() override;
    void Draw() const override;
    void Update(float deltaTime) override;

    void SetUniqueName(const std::string & name);
    const std::string& GetUniqueName() const;

    void Body1CollisionEvent(Object *Other, BodyCollisionSide Side);
    void Body2CollisionEvent(Object *Other, BodyCollisionSide Side);

};

#endif //PLAYER_HPP