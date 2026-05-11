#ifndef PROJECTILEOBJECT_HPP
#define PROJECTILEOBJECT_HPP

#include "../WandAllegroEngine/Models/GameObject.hpp"
#include "../WandAllegroEngine/Models/GameTimer.hpp"

using namespace WandEngine;

class ProjectileObject : public GameObject
{
private:
    float Velocity;
    float directionX;
    float directionY;
    GameObject* Owner;
    GameTimer* AliveTimer;
public:
    ProjectileObject(Object ObjectType, WAND_VEC2 Position, WAND_VEC2 Size, std::string ObjectName, ALLEGRO_BITMAP *Sprite, short int LayerId, GameObject* Owner, WAND_VEC2 MoveToPosition);
    void Draw() override;
    void Init() override;
    void Update(float DeltaTime) override;
    void OnCollision(GameObject *Other) override;

    void MoveToPosition(WAND_VEC2 NewPosition);

};


#endif // PROJECTILEOBJECT_HPP