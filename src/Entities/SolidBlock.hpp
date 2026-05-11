#ifndef SOLIDBLOCK_HPP
#define SOLIDBLOCK_HPP

#include <WandEngine/Models/GameObject.hpp>
#include <WandEngine/Models/GameBodyObject.hpp>

using namespace WandEngine;

class SolidBlock : public GameObject
{
private:
    GameBodyObject* Body;
    int BlockID;
    
public:
    SolidBlock(int BlockID, glm::vec2 Position, glm::vec2 Size, short int LayerId);
    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;

    virtual SolidBlock* Clone() const override
    {
        return new SolidBlock(*this);
    }

    void BodyCollisionEvent(GameObject *other, CollisionSide Side);
    static void SaveToDB();
    static void LoadFrom();

    int GetBlockID();

};

#endif //SOLIDBLOCK_HPP