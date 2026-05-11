#ifndef ORB_HPP
#define ORB_HPP

#include <WandEngine/Models/Object/Object.hpp>
#include <WandEngine/Models/Body/Body.hpp>

using namespace WandEngine;

enum class OrbType
{
    Green,
    Blue,
    White,
    Orange,
    Black
};

class Orb : public Object
{
private:
    Body* body;
    OrbType Type;
    bool Used;

public:
    Orb(OrbType Type, glm::vec2 Position, glm::vec2 Size, short int LayerId);
    ~Orb();
    void Init() override;
    void Draw() const override;
    void Update(float deltaTime) override;
    void BodyCollisionEvent(Object *other, BodyCollisionSide Side);

    void SetUsed();
    bool IsUsed();
    OrbType GetOrbType();

    virtual Orb* Clone() const override
    {
        return new Orb(*this);
    }

    std::vector<std::string> GetAllValues() const;
    static void RegisterSerialize();
};

#endif //ORB_HPP