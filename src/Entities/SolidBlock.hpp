#ifndef SOLIDBLOCK_HPP
#define SOLIDBLOCK_HPP

#include <WandEngine/Models/Object/Object.hpp>
#include <WandEngine/Models/Body/Body.hpp>

using namespace WandEngine;

class SolidBlock : public Object
{
private:
    Body* body;
    int BlockID;
    
public:
    SolidBlock(int BlockID, glm::vec2 Position, glm::vec2 Size, short int LayerId);
    void Init() override;
    void Draw() const override;
    void Update(float deltaTime) override;

    virtual SolidBlock* Clone() const override
    {
        return new SolidBlock(*this);
    }

    void BodyCollisionEvent(Object *other, BodyCollisionSide Side);

    std::vector<std::string> GetAllValues() const;
    static void RegisterSerialize();

    int GetBlockID();

};

#endif //SOLIDBLOCK_HPP