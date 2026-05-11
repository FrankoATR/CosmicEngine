#ifndef CUSTOMENEMY_HPP
#define CUSTOMENEMY_HPP

#include "../WandAllegroEngine/Models/GameObject.hpp"
#include "../WandAllegroEngine/Models/GameTimer.hpp"

using namespace WandEngine;

class CustomEnemy : public GameObject
{
private:

    enum class MoveDirection {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        UP_LELFT,
        UP_RIGHT,
        DOWN_LEFT,
        DOWM_RIGHT
    }directions[8] = {MoveDirection::UP, MoveDirection::DOWN, MoveDirection::LEFT, MoveDirection::RIGHT, MoveDirection::UP_LELFT, MoveDirection::UP_RIGHT, MoveDirection::DOWN_LEFT, MoveDirection::DOWM_RIGHT};

    ALLEGRO_FONT* font;
    int HP;
    //GameTimer* MoveTimer;
    MoveDirection CurrentDirection;
    bool InitFollow;

public:
    CustomEnemy(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font);
    ~CustomEnemy();
    void Draw() override;
    void Init() override;
    void Update(float deltaTime) override;
    void OnCollision(GameObject* other) override;
    
    void MoveUp(float deltaTime);
    void MoveDown(float deltaTime);
    void MoveRight(float deltaTime);
    void MoveLeft(float deltaTime);
    void MoveUpLeft(float deltaTime);
    void MoveUpRight(float deltaTime);
    void MoveDownLeft(float deltaTime);
    void MoveDownRight(float deltaTime);
};



#endif // CUSTOMENEMY_HPP