#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <WandEngine/Models/GameObject.hpp>

using namespace WandEngine;

class Background : public GameObject
{
private:

public:
    Background(glm::vec2 Position, glm::vec2 Size, short int LayerId);
    void Init() override;
    void Draw() const override;
    void Update(float deltaTime) override;

};

#endif //BACKGROUND_HPP