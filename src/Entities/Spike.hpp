#ifndef SPIKE_HPP
#define SPIKE_HPP

#include <WandEngine/Models/Object/Object.hpp>
#include <WandEngine/Models/Body/Body.hpp>

using namespace WandEngine;

enum class SpikeType
{
    Big,
    Medium,
    Small,
    Diminute
};

class Spike : public Object
{
private:
    SpikeType Type;

    Body *body;

public:

    Spike(SpikeType Type, glm::vec2 Position, glm::vec2 Size, short int LayerId);
    void Init() override;
    void Draw() const override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(Object *other, BodyCollisionSide Side);

    virtual Spike* Clone() const override
    {
        return new Spike(*this);
    }

    
    std::vector<std::string> GetAllValues() const;
    static void RegisterSerialize();

    SpikeType GetSpikeType();
};

#endif // SPIKE_HPP