#ifndef NABAZU_ENEMY_BULLET_HPP
#define NABAZU_ENEMY_BULLET_HPP

#include <CosmicEngine/models/body/body.hpp>
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/timer/timer.hpp>

#include <glm/glm.hpp>

#include <string>

namespace NaBazu
{
    // Enemy-fired projectile -- same skeleton as the player's Bullet (engine-integrated
    // SetVelocity + a small centered Body, see the -size/2 offset note in CreateBody),
    // but only reacts to the Ship (never damages other enemies) and uses a distinct
    // color so the player can tell enemy fire apart from their own shots.
    class EnemyBullet : public CosmicEngine::Object
    {
    public:
        static const std::string &StaticClassName();

        EnemyBullet(glm::vec3 spawnPosition, glm::vec3 direction, int damage, float speed);
        ~EnemyBullet() override;

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
