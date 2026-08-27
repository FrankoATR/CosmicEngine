#ifndef NABAZU_MISSILE_HPP
#define NABAZU_MISSILE_HPP

#include <CosmicEngine/models/body/body.hpp>
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/timer/timer.hpp>

#include <glm/glm.hpp>

#include <string>

namespace NaBazu
{
    // Secondary weapon projectile. Deals no direct-hit damage; both a Body collision
    // and lifetime expiry trigger Explode(), which applies flat splash damage to every
    // enemy within splashRadius_ via a manual glm::distance check -- CosmicEngine's
    // collision system is AABB-only, so there is no sphere collider to lean on.
    class Missile : public CosmicEngine::Object
    {
    public:
        static const std::string &StaticClassName();

        Missile(glm::vec3 spawnPosition, glm::vec3 direction, float speed = 90.0f,
                float splashRadius = 24.0f, int splashDamage = 60); // radius doubled (12 -> 24)
        ~Missile() override;

        void update(float deltaTime) override;
        void draw() const override;

    private:
        void CreateBody();
        void OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side);
        void Explode();

        float splashRadius_;
        int splashDamage_;
        int bodyId_;
        bool exploded_;
        CosmicEngine::Timer *lifetimeTimer_;
    };
}

#endif
