#ifndef NABAZU_PICKUP_HPP
#define NABAZU_PICKUP_HPP

#include <CosmicEngine/models/body/body.hpp>
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/timer/timer.hpp>

#include <glm/glm.hpp>

#include <string>

namespace NaBazu
{
    enum class PickupKind
    {
        Ammo,    // 3-pointed star
        Missile, // 5-pointed star
        Health   // red ring; restores a percentage of max health rather than a count
    };

    // Ammo/missile/health pickup dropped by destroyed enemies. Object exposes no
    // public angular-velocity setter, so the cosmetic spin is done manually in
    // update() (the same pattern the engine's own SandboxHammerObject3D uses), not
    // via the automatic velocity integration used by Bullet/Missile.
    class Pickup : public CosmicEngine::Object
    {
    public:
        static const std::string &StaticClassName();

        Pickup(glm::vec3 position, PickupKind kind, int amount);
        ~Pickup() override;

        void update(float deltaTime) override;
        void draw() const override;

    private:
        void CreateBody();
        void OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side);

        PickupKind kind_;
        int amount_;
        int bodyId_;
        float spinSpeedDeg_;
        CosmicEngine::Timer *lifetimeTimer_;
    };
}

#endif
