#include "bullet.hpp"

#include "enemy_grunt.hpp"
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
    const std::string &Bullet::StaticClassName()
    {
        static const std::string className = "Bullet";
        return className;
    }

    Bullet::Bullet(glm::vec3 spawnPosition, glm::vec3 direction, int damage, float speed)
        : Object(StaticClassName(), spawnPosition, glm::vec3(0.4f)),
          damage_(damage),
          bodyId_(-1),
          lifetimeTimer_(nullptr)
    {
        SetColor(glm::vec3(1.0f, 0.9f, 0.2f));

        const glm::vec3 travelDirection = (glm::length(direction) > 1e-5f) ? glm::normalize(direction) : glm::vec3(0.0f, 0.0f, -1.0f);
        SetVelocity(travelDirection * speed);

        // Face the bolt mesh's tip (local -Z, see BuildUnitBoltVertices) along the
        // travel direction -- same yaw/pitch formula Ship uses for its -Z-forward
        // nose, reused verbatim since it is already proven to orient a -Z-forward
        // mesh correctly. Roll is irrelevant: the bolt is rotationally symmetric
        // around its own axis.
        const float horizLen = std::sqrt(travelDirection.x * travelDirection.x + travelDirection.z * travelDirection.z);
        const float yawDeg = glm::degrees(std::atan2(-travelDirection.x, -travelDirection.z));
        const float pitchDeg = glm::degrees(std::atan2(travelDirection.y, horizLen));
        SetRotation(glm::vec3(pitchDeg, yawDeg, 0.0f));

        // 2.0s at 275 u/s (~234 u/s relative to the forward-moving ship) keeps every
        // bullet inside the collision window for its whole life -- see
        // kCollisionWindowSize in gameplay_scene.cpp. A longer life would just create
        // bullets that fly on uselessly with collision silently disabled.
        lifetimeTimer_ = new CosmicEngine::Timer(2.0, false, false);
        CosmicEngine::TimerManager::GetInstance().Add(lifetimeTimer_);

        CreateBody();
    }

    Bullet::~Bullet()
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

    void Bullet::CreateBody()
    {
        // See Ship::CreateBody: rendering treats GetPosition() as the box center,
        // collision treats a Body's position as its min corner -- offset by -size/2
        // so the two agree. Hitbox is intentionally larger than the visual mesh
        // (kHitboxScale) -- bullets are tiny (0.4 units) and fast, so a forgiving
        // hitbox matters more than pixel-perfect visual/collision alignment here.
        constexpr float kHitboxScale = 3.0f;
        const glm::vec3 hitboxSize = GetSize() * kHitboxScale;
        CosmicEngine::Body *body = new CosmicEngine::Body(
            this,
            -hitboxSize * 0.5f,
            hitboxSize,
            CALLBACK_COLLISION_EVENT(OnBodyCollision));

        BOD_MN.Add(body);
        bodyId_ = body->GetID();
    }

    void Bullet::OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side)
    {
        (void)side;

        if (!other)
        {
            return;
        }

        if (other->GetClassName() == EnemyGrunt::StaticClassName())
        {
            static_cast<EnemyGrunt *>(other)->TakeDamage(damage_);
            Destroy();
        }
    }

    void Bullet::update(float deltaTime)
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

    void Bullet::draw() const
    {
        // Visual size only -- GetSize() itself (what CreateBody's hitbox is derived
        // from) is untouched, so doubling the render scale here does not change
        // collision at all.
        const float elapsedTime = lifetimeTimer_ ? static_cast<float>(lifetimeTimer_->GetElapsedTime()) : 0.0f;
        BoltGlowRenderer::GetInstance().Draw(GetPosition(), GetSize() * 2.0f, GetRotation(), GetColor(), elapsedTime);
    }
}
