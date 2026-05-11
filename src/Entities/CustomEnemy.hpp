#ifndef CUSTOMENEMY_HPP
#define CUSTOMENEMY_HPP

#include "../WandAllegroEngine/Models/GameObject.hpp"

using namespace WandEngine;

class CustomEnemy : public GameObject
{
private:

    enum class Direction {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        UP_LELFT,
        UP_RIGHT,
        DOWN_LEFT,
        DOWM_RIGHT
    }directions[8] = {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT, Direction::UP_LELFT, Direction::UP_RIGHT, Direction::DOWN_LEFT, Direction::DOWM_RIGHT};

    //std::vector<GameObject*> inventory;
    ALLEGRO_FONT* font;
    int HP;
    int TimeToChangeDirection;
    int TimeUntilRecibeDamage;
    double last_time;
    Direction ActualDirection;
    float Velocity;

public:
    CustomEnemy(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font);
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