#include "enemy_bullet.hpp"

#include "ship.hpp"
#include "../systems/glow_mesh_renderer.hpp"
#include "../utilities/nabazu_asset_defines.hpp"

#include "../systems/test_mode.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include BODYMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include TIMERMANAGER_HEADER

#include <cmath>

namespace NaBazu
{
    const std::string &EnemyBullet::StaticClassName()
    {
        static const std::string className = "EnemyBullet";
        return className;
    }

    EnemyBullet::EnemyBullet(glm::vec3 spawnPosition, glm::vec3 direction, int damage, float speed)
        : Object(StaticClassName(), spawnPosition, glm::vec3(0.5f)),
          damage_(damage),
          bodyId_(-1),
          lifetimeTimer_(nullptr)
    {
        SetColor(glm::vec3(1.0f, 0.15f, 0.85f));

        const glm::vec3 travelDirection = (glm::length(direction) > 1e-5f) ? glm::normalize(direction) : glm::vec3(0.0f, 0.0f, 1.0f);
        SetVelocity(travelDirection * speed);

        // See Bullet's constructor: same -Z-forward bolt-facing formula, reused
        // verbatim so enemy shots use the identical mesh/shader as the player's.
        const float horizLen = std::sqrt(travelDirection.x * travelDirection.x + travelDirection.z * travelDirection.z);
        const float yawDeg = glm::degrees(std::atan2(-travelDirection.x, -travelDirection.z));
        const float pitchDeg = glm::degrees(std::atan2(travelDirection.y, horizLen));
        SetRotation(glm::vec3(pitchDeg, yawDeg, 0.0f));

        lifetimeTimer_ = new CosmicEngine::Timer(3.0, false, false);
        CosmicEngine::TimerManager::GetInstance().Add(lifetimeTimer_);

        CreateBody();
    }

    EnemyBullet::~EnemyBullet()
    {
        if (bodyId_ >= 0)
        {
            BOD_MN.Remove(bodyId_);
            bodyId_ = -1;
        }

        if (lifetimeTimer_)
        {
            lifetimeTimer_->Destroy();
            lifetimeTimer_ = nullptr;
        }
    }

    void EnemyBullet::CreateBody()
    {
        // See Ship::CreateBody: rendering treats GetPosition() as the box center,
        // collision treats a Body's position as its min corner -- offset by -size/2
        // so the two agree.
        CosmicEngine::Body *body = new CosmicEngine::Body(
            this,
            -GetSize() * 0.5f,
            GetSize(),
            CALLBACK_COLLISION_EVENT(OnBodyCollision));

        BOD_MN.Add(body);
        bodyId_ = body->GetID();
    }

    void EnemyBullet::OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side)
    {
        (void)side;

        if (!other)
        {
            return;
        }

        if (other->GetClassName() == Ship::StaticClassName())
        {
            static_cast<Ship *>(other)->TakeDamage(damage_);
            Destroy();
        }
    }

    void EnemyBullet::update(float deltaTime)
    {
        // TEMPORARY test mode: hold position (and skip lifetime expiry) while the
        // world is frozen for collision inspection.
        if (IsWorldFrozen())
        {
            return;
        }

        (void)deltaTime;

        if (lifetimeTimer_ && lifetimeTimer_->IsFinished())
        {
            Destroy();
        }
    }

    void EnemyBullet::draw() const
    {
        // Same BoltGlowRenderer as the player's Bullet (per request: enemy shots use
        // the same mesh/shader), tinted with this entity's own magenta color. Visual
        // size only -- GetSize() itself (what CreateBody's hitbox is derived from) is
        // untouched, so doubling the render scale here does not change collision.
        const float elapsedTime = lifetimeTimer_ ? static_cast<float>(lifetimeTimer_->GetElapsedTime()) : 0.0f;
        BoltGlowRenderer::GetInstance().Draw(GetPosition(), GetSize() * 2.0f, GetRotation(), GetColor(), elapsedTime);
    }
}
