#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <WandEngine/Models/GameObject.hpp>
#include <WandEngine/Managers/TimerManager.hpp>
#include <WandEngine/Models/UIElements/UIText.hpp>

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
    bool OnGroundOrBlock;
    float Gravity;
    GameTimer* RotateSprite_Timer;
    GameTimer* Update_Gravity_Timer;
    GameBodyObject* Body1;
    GameBodyObject* Body2;
    GameBodyObject* Center;

    UIText* Label_Info1;

public:
    Player(PlayerMode mode, WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId);
    ~Player();

    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;
    void Body1CollisionEvent(GameObject *Other, CollisionSide Side);
    void Body2CollisionEvent(GameObject *Other, CollisionSide Side);

};

#endif //PLAYER_HPP