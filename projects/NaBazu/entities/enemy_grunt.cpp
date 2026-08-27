#include "enemy_grunt.hpp"

#include "enemy_bullet.hpp"
#include "explosion_effect.hpp"
#include "pickup.hpp"
#include "ship.hpp"
#include "../utilities/nabazu_asset_defines.hpp"
#include "../utilities/nabazu_models.hpp"
#include "../utilities/random_utils.hpp"

#include "../systems/test_mode.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include AUDIOMANAGER_HEADER
#include BODYMANAGER_HEADER
#include EVENTMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include TIMERMANAGER_HEADER

#include <cmath>

namespace
{
    constexpr float kMissileDropChance = 0.08f;
    constexpr float kHealthDropChance = 0.07f;
    constexpr float kAmmoDropChance = 0.25f;
    constexpr int kAmmoDropAmount = 30;
    constexpr int kMissileDropAmount = 2;
    // For PickupKind::Health the amount is a PERCENTAGE of max health, not a count.
    constexpr int kHealthDropPercent = 25;
    constexpr int kShotDamage = 8;
}

namespace NaBazu
{
    const std::string &EnemyGrunt::StaticClassName()
    {
        static const std::string className = "EnemyGrunt";
        return className;
    }

    EnemyGrunt::EnemyGrunt(glm::vec3 spawnPosition, float railDistance, EnemyShotPattern shotPattern,
                           float shotSpeed, int health, float size, int modelIndex)
        : Object(StaticClassName(), spawnPosition, glm::vec3(size)),
          spawnOrigin_(spawnPosition),
          railDistance_(railDistance),
          bobPhase_(NaBazu::RandomRange(0.0f, 6.28f)),
          bobAmplitude_(1.0f),
          bobSpeed_(2.0f),
          health_(health),
          maxHealth_(health),
          ramDamage_(15),
          bodyId_(-1),
          modelIndex_(modelIndex),
          facingYawDeg_(0.0f),
          dead_(false),
          shotPattern_(shotPattern),
          shotSpeed_(shotSpeed),
          fireCooldown_(nullptr)
    {
        SetColor(glm::vec3(1.0f, 0.2f, 0.2f));
        CreateBody();

        // Face the player at spawn. Uses the real direction to the ship rather than a
        // fixed +Z so craft on a curving stretch of rail still look down the corridor.
        glm::vec3 towardPlayer(0.0f, 0.0f, 1.0f);
        auto ships = OBJ_MN.FindByClassName(Ship::StaticClassName());
        if (!ships.empty() && ships.front())
        {
            const glm::vec3 toShip = ships.front()->GetPosition() - spawnPosition;
            if (glm::length(toShip) > 1e-4f)
            {
                towardPlayer = glm::normalize(toShip);
            }
        }
        // atan2(-x,-z) yields the yaw that points local -Z along a direction; the
        // models are nose-along +Z, hence the extra kModelYawOffsetDeg (see
        // utilities/nabazu_models.hpp).
        facingYawDeg_ = glm::degrees(std::atan2(-towardPlayer.x, -towardPlayer.z)) + kModelYawOffsetDeg;

        if (shotPattern_ != EnemyShotPattern::None)
        {
            // Target time 0 -- IsFinished() becomes true after the first tick, same
            // "ready immediately" pattern used by Ship's weapon cooldowns.
            fireCooldown_ = new CosmicEngine::Timer(0.0, false, false);
            CosmicEngine::TimerManager::GetInstance().Add(fireCooldown_);
        }
    }

    EnemyGrunt::~EnemyGrunt()
    {
        if (bodyId_ >= 0)
        {
            BOD_MN.Remove(bodyId_);
            bodyId_ = -1;
        }

        if (fireCooldown_)
        {
            fireCooldown_->Destroy();
            fireCooldown_ = nullptr;
        }
    }

    void EnemyGrunt::CreateBody()
    {
        // See Ship::CreateBody: rendering treats GetPosition() as the box center,
        // collision treats a Body's position as its min corner -- offset by -size/2
        // so the two agree. The box matches the logical size 1:1; the drawn model is
        // 1.4x that, so the hitbox sits just inside the visible hull.
        constexpr float kHitboxScale = 1.0f;
        const glm::vec3 hitboxSize = GetSize() * kHitboxScale;
        CosmicEngine::Body *body = new CosmicEngine::Body(
            this,
            -hitboxSize * 0.5f,
            hitboxSize,
            CALLBACK_COLLISION_EVENT(OnBodyCollision));

        BOD_MN.Add(body);
        bodyId_ = body->GetID();
    }

    void EnemyGrunt::TakeDamage(int amount)
    {
        if (dead_)
        {
            return;
        }

        health_ -= amount;
        if (health_ <= 0)
        {
            OnDeath();
        }
    }

    float EnemyGrunt::GetRailDistance() const
    {
        return railDistance_;
    }

    void EnemyGrunt::MaybeDespawnBehind(float shipDistanceTraveled, float despawnMargin)
    {
        if (!dead_ && shipDistanceTraveled - railDistance_ > despawnMargin)
        {
            Destroy();
        }
    }

    void EnemyGrunt::OnDeath()
    {
        if (dead_)
        {
            return;
        }
        dead_ = true;

        const float roll = NaBazu::RandomFloat01();
        if (roll < kMissileDropChance)
        {
            OBJ_MN.Add(new Pickup(GetPosition(), PickupKind::Missile, kMissileDropAmount));
        }
        else if (roll < kMissileDropChance + kHealthDropChance)
        {
            OBJ_MN.Add(new Pickup(GetPosition(), PickupKind::Health, kHealthDropPercent));
        }
        else if (roll < kMissileDropChance + kHealthDropChance + kAmmoDropChance)
        {
            OBJ_MN.Add(new Pickup(GetPosition(), PickupKind::Ammo, kAmmoDropAmount));
        }

        OBJ_MN.Add(new ExplosionEffect(GetPosition(), 2.5f));
        AUD_MN.PlayAt(NABAZU_SFX_ENEMY_DESTROY_KEY, GetPosition(), CosmicEngine::SoundSpace::World3D, 0.8f, false, true);

        // Tougher enemies are worth more points. TriggerEvent on a name with no
        // registered listener just logs a warning (see event_manager.cpp), so this is
        // safe even before GameplayScene has registered its score listener.
        EVT_MN.TriggerEvent<int>("nabazu.enemy_killed", 10 + maxHealth_ / 2);

        Destroy();
    }

    void EnemyGrunt::OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side)
    {
        (void)side;

        if (!other || dead_)
        {
            return;
        }

        if (other->GetClassName() == Ship::StaticClassName())
        {
            static_cast<Ship *>(other)->TakeDamage(ramDamage_);
            OnDeath();
        }
    }

    void EnemyGrunt::UpdateFiring(float deltaTime)
    {
        (void)deltaTime;

        if (shotPattern_ == EnemyShotPattern::None || !fireCooldown_ || !fireCooldown_->IsFinished())
        {
            return;
        }

        glm::vec3 direction;
        if (shotPattern_ == EnemyShotPattern::Forward)
        {
            // Back down the corridor, toward where the ship is coming from.
            direction = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        else
        {
            auto ships = OBJ_MN.FindByClassName(Ship::StaticClassName());
            if (ships.empty() || !ships.front())
            {
                return; // no target this tick, try again once the cooldown re-fires
            }
            const glm::vec3 toShip = ships.front()->GetPosition() - GetPosition();
            direction = (glm::length(toShip) > 1e-5f) ? glm::normalize(toShip) : glm::vec3(0.0f, 0.0f, 1.0f);
        }

        OBJ_MN.Add(new EnemyBullet(GetPosition(), direction, kShotDamage, shotSpeed_));

        const float nextDelay = (shotPattern_ == EnemyShotPattern::Forward)
                                     ? NaBazu::RandomRange(1.5f, 2.5f)
                                     : NaBazu::RandomRange(2.0f, 3.5f);
        fireCooldown_->SetTargetTime(nextDelay);
        fireCooldown_->reset();
        fireCooldown_->Play();
    }

    void EnemyGrunt::update(float deltaTime)
    {
        // TEMPORARY test mode: hold position (and skip lifetime expiry) while the
        // world is frozen for collision inspection.
        if (IsWorldFrozen())
        {
            return;
        }

        if (dead_)
        {
            return;
        }

        bobPhase_ += bobSpeed_ * deltaTime;
        SetPosition(spawnOrigin_ + glm::vec3(0.0f, std::sin(bobPhase_) * bobAmplitude_, 0.0f));

        UpdateFiring(deltaTime);
    }

    void EnemyGrunt::draw() const
    {
        // facingYawDeg_ already encodes both "turn toward the player" and the models'
        // +Z-nose authoring offset (resolved once at spawn).
        const glm::vec3 facingPlayer(GetRotation().x, facingYawDeg_, GetRotation().z);
        // 25% smaller than the previous 1.4x factor; GetSize() (the hitbox) is
        // deliberately left alone, so the collision box sits slightly outside the
        // drawn craft -- shots land generously rather than pixel-exactly.
        DrawShipModel(GetEnemyModel(modelIndex_), GetPosition(), GetSize().x * 1.05f, facingPlayer);
    }
}
