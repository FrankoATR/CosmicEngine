#ifndef CUBE_HPP
#define CUBE_HPP

#include <WandEngine/Models/Object/Object.hpp>
#include <WandEngine/Models/Body/Body.hpp>

using namespace WandEngine;

class Cube : public Object
{
private:
    Body *body;

public:

    Cube(glm::vec3 Position, glm::vec3 Size);
    void Init() override;
    void Draw() const override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(Object *other, BodyCollisionSide Side);

    virtual Cube* Clone() const override
    {
        return new Cube(*this);
    }
    
    std::vector<std::string> GetAllValues() const;
    static void RegisterSerialize();
};

#endif // CUBE_HPP