#ifndef ENDLEVEL_HPP
#define ENDLEVEL_HPP

#include <WandEngine/Models/GameObject.hpp>

using namespace WandEngine;

class EndLevel : public GameObject
{
private:
    GameBodyObject* Body;

public:
    EndLevel(WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId);
    ~EndLevel();
    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(GameObject *other, CollisionSide Side);
};

#endif //ENDLEVEL_HPP