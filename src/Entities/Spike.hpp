#ifndef SPIKE_HPP
#define SPIKE_HPP

#include <WandEngine/Models/GameObject.hpp>

using namespace WandEngine;

class Spike : public GameObject
{
private:
    GameBodyObject* Body;

public:
    Spike(WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId);
    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(GameObject *other, CollisionSide Side);
};

#endif //SPIKE_HPP