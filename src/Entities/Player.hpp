#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <WandEngine/Models/GameObject.hpp>
#include <WandEngine/Models/GameBodyObject.hpp>
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

class Player : public GameObject
{
private:
    std::string UniqueName;

    bool OnGroundOrBlock;
    float Gravity;

    GameBodyObject* Body1;
    GameBodyObject* Body2;

    bool KeySpaceDown, KeySpaceRelease;
    bool RigthClickDown, RigthClickRelease;
    bool ReleaseJumping;

    PlayerMode CurrentPlayerMode;

    std::random_device rd;

    GameTimer* TimeToEndTimer;

    int randomNumber1;

public:
    Player(std::string UniqueName, PlayerMode mode, glm::vec2 Position, glm::vec2 Size, float Rotation, short int LayerId);
    ~Player();

    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;

    void SetUniqueName(const std::string & name);
    const std::string& GetUniqueName() const;

    void Body1CollisionEvent(GameObject *Other, CollisionSide Side);
    void Body2CollisionEvent(GameObject *Other, CollisionSide Side);

};

#endif //PLAYER_HPP