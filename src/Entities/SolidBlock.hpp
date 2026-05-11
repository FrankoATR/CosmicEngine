#ifndef SOLIDBLOCK_HPP
#define SOLIDBLOCK_HPP

#include <WandEngine/Models/GameObject.hpp>

using namespace WandEngine;

class SolidBlock : public GameObject
{
private:
    GameBodyObject* Body;
    int randomNumber1;
    int randomNumber2;
    
public:
    SolidBlock(glm::vec2 Position, glm::vec2 Size, short int LayerId);
    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(GameObject *other, CollisionSide Side);
};

#endif //SOLIDBLOCK_HPP