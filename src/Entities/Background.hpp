#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include <WandEngine/Models/Object/Object.hpp>

using namespace WandEngine;

class Background : public Object
{
public:
    Background(glm::vec2 position, glm::vec2 size, short int layerId);
    void Init() override;
    void Draw() const override;
    void Update(float deltaTime) override;

};

#endif //BACKGROUND_HPP