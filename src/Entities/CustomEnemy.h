#ifndef CUSTOMENEMY_H
#define CUSTOMENEMY_H

#include "../WandAllegroEngine/Models/GameObject.h"

class CustomEnemy : public GameObject
{
private:

    enum Direction {
        LEFT,
        RIGHT
    };

    //std::vector<GameObject*> inventory;
    ALLEGRO_FONT* font;
    int HP;
    float TimeToChangeDirection;
    float TimeUntilRecibeDamage;
    Direction ActualDirection;

public:
    CustomEnemy(GameManager* Game, Object ObjectType, Vec2 Position, Vec2 Size, std::string ObjectName, ALLEGRO_BITMAP* Sprite, short int LayerId, int HP, ALLEGRO_FONT* font);
    void Draw() override;
    void Init() override;
    void Update(float deltaTime) override;
    void OnCollision(GameObject* other) override;
    void MoveRight(float deltaTime);
    void MoveLeft(float deltaTime);

};



#endif