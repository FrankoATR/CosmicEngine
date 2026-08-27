#ifndef NABAZU_BULLET_HPP
#define NABAZU_BULLET_HPP

#include <CosmicEngine/models/body/body.hpp>
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/timer/timer.hpp>

#include <glm/glm.hpp>

#include <string>

namespace NaBazu
{
    // Primary weapon projectile. Uses the engine's built-in velocity integration
    // (unlike Ship, which drives its own transform) since straight-line motion is
    // exactly what Object::SetVelocity + UpdatePosition already provides.
    class Bullet : public CosmicEngine::Object
    {
    public:
        static const std::string &StaticClassName();

        Bullet(glm::vec3 spawnPosition, glm::vec3 direction, int damage = 10, float speed = 275.0f); // 220 * 1.25
        ~Bullet() override;

        void update(float deltaTime) override;
        void draw() const override;

    private:
        void CreateBody();
        void OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side);

        int damage_;
        int bodyId_;
        CosmicEngine::Timer *lifetimeTimer_;
    };
}

#endif
