#ifndef SPIKE_HPP
#define SPIKE_HPP

#include <WandEngine/Models/GameObject.hpp>
#include <WandEngine/Models/GameBodyObject.hpp>

using namespace WandEngine;

enum class SpikeType
{
    Big,
    Medium,
    Small,
    Diminute
};

class Spike : public GameObject
{
private:
    SpikeType Type;

    GameBodyObject *Body;

public:

    Spike(SpikeType Type, glm::vec2 Position, glm::vec2 Size, short int LayerId);
    void Init() override;
    void Draw() override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(GameObject *other, CollisionSide Side);

    virtual Spike* Clone() const override
    {
        return new Spike(*this);
    }

    static void SaveToDB();
    static void LoadFrom();

    SpikeType GetSpikeType();
};

#endif // SPIKE_HPP