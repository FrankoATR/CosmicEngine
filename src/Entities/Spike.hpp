#ifndef SPIKE_HPP
#define SPIKE_HPP

#include <WandEngine/Models/GameObject.hpp>

using namespace WandEngine;

class Spike : public GameObject
{
private:
    GameBodyObject* Body;
    int randomNumber1;
    
public:
    Spike(glm::vec2 Position, glm::vec2 Size, short int LayerId);
    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(GameObject *other, CollisionSide Side);
};

#endif //SPIKE_HPP