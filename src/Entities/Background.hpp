#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <WandEngine/Models/GameObject.hpp>

using namespace WandEngine;

class Background : public GameObject
{
private:

public:
    Background(std::string UniqueName, glm::vec2 Position, glm::vec2 Size, short int LayerId);
    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;

};

#endif //BACKGROUND_HPP