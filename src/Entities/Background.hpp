#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <WandEngine/Models/GameObject.hpp>

using namespace WandEngine;

class Background : public GameObject
{
private:

public:
    Background(WAND_VEC2 Position, WAND_VEC2 Size, ALLEGRO_BITMAP *Sprite, short int LayerId);
    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;

};

#endif //BACKGROUND_HPP