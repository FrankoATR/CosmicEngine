#ifndef GROUND_HPP
#define GROUND_HPP

#include <WandEngine/Models/GameObject.hpp>

using namespace WandEngine;

class Ground : public GameObject
{
private:
    GameBodyObject* Body;

public:
    Ground(WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId);
    void Draw() override;
    void Init() override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(GameObject *other, CollisionSide Side);

};

#endif //GROUND_HPP