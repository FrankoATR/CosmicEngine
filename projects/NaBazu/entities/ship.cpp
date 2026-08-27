#include "ship.hpp"

#include "bullet.hpp"
#include "enemy_grunt.hpp"
#include "missile.hpp"
#include "../systems/glow_mesh_renderer.hpp"
#include "../utilities/math_utils.hpp"
#include "../utilities/nabazu_asset_defines.hpp"
#include "../utilities/nabazu_input_actions.hpp"
#include "../utilities/nabazu_models.hpp"

// Fase 2: nave mas rapida, 3x el rango de movimiento, giro real (yaw) al esquivar
// (no solo banking cosmetico), y disparo/reticula alineados con la direccion real
// de apuntado en vez de la tangente del riel -- ver plan, seccion 1.

#include <CosmicEngine/interfaces/definitions.hpp>
#include AUDIOMANAGER_HEADER
#include BODYMANAGER_HEADER
#include INPUTMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include TIMERMANAGER_HEADER

#include <algorithm>
#include <cmath>

namespace
{
    // Max extra pitch/yaw/roll (degrees) layered on top of the rail's own orientation
    // in response to steering input. Pitch and yaw actually turn the nose (and thus
    // GetAimDirection()); roll is purely cosmetic banking (it doesn't change the local
    // forward axis, confirmed against the renderer's Rx*Ry*Rz composition order).
    constexpr float kMaxSteerPitchDeg = 12.0f;
    constexpr float kMaxSteerYawDeg = 20.0f;
    constexpr float kMaxBankDeg = 35.0f;

    constexpr float kReticleDistance = 45.0f;
    constexpr float kReticleSize = 3.6f; // 4x the original 0.9 (300% bigger)
}

namespace NaBazu
{
    const std::string &Ship::StaticClassName()
    {
        static const std::string className = "Ship";
        return className;
    }

    Ship::Ship(RailPath *rail, glm::vec3 initialOffset)
        : Object(StaticClassName(), rail ? rail->GetFrameAtDistance(0.0f).position : glm::vec3(0.0f), glm::vec3(9.0f)),
          rail_(rail),
          bodyId_(-1),
          autoAdvanceEnabled_(true),
          elapsedTime_(0.0f),
          aimLockStrength_(0.0f),
          lastAssistTargetDir_(0.0f, 0.0f, -1.0f),
          assistedAimDirection_(0.0f, 0.0f, -1.0f),
          distanceTraveled_(0.0f),
          cruiseSpeed_(41.25f), // 25% slower than the previous 55.0f
          brakeSpeed_(15.0f),   // scaled down with cruiseSpeed_ to keep the same relative feel
          currentSpeed_(41.25f),
          lateralOffset_(initialOffset.x),
          verticalOffset_(initialOffset.y),
          lateralOffsetMax_(42.0f),
          verticalOffsetMax_(30.0f),
          steeringSpeed_(42.5f), // 50% slower lateral/vertical movement; cruiseSpeed_ (forward) is untouched
          lastLateralInput_(0.0f),
          lastVerticalInput_(0.0f),
          currentFrame_(),
          noseOffsetLocal_(0.0f, 0.0f, -9.0f),
          wingOffsetLocal_{glm::vec3(-6.0f, -0.75f, -3.0f), glm::vec3(6.0f, -0.75f, -3.0f)},
          brakeState_(BrakeState::Idle),
          brakeStateTimer_(nullptr),
          health_(100),
          maxHealth_(100),
          ammo_(600),    // quadruple the previous base ammo (150 -> 600)
          maxAmmo_(600),
          missiles_(6),
          maxMissiles_(6),
          primaryFireCooldown_(nullptr),
          missileFireCooldown_(nullptr),
          nextHardpointIsLeft_(true)
    {
        SetColor(glm::vec3(0.2f, 0.85f, 1.0f));

        brakeStateTimer_ = new CosmicEngine::Timer(0.0, false, true);
        CosmicEngine::TimerManager::GetInstance().Add(brakeStateTimer_);

        primaryFireCooldown_ = new CosmicEngine::Timer(0.0, false, false);
        CosmicEngine::TimerManager::GetInstance().Add(primaryFireCooldown_);

        missileFireCooldown_ = new CosmicEngine::Timer(0.0, false, false);
        CosmicEngine::TimerManager::GetInstance().Add(missileFireCooldown_);

        CreateBody();
    }

    Ship::~Ship()
    {
        if (bodyId_ >= 0)
        {
            BOD_MN.Remove(bodyId_);
            bodyId_ = -1;
        }

        if (brakeStateTimer_)
        {
            brakeStateTimer_->Destroy();
            brakeStateTimer_ = nullptr;
        }

        if (primaryFireCooldown_)
        {
            primaryFireCooldown_->Destroy();
            primaryFireCooldown_ = nullptr;
        }

        if (missileFireCooldown_)
        {
            missileFireCooldown_->Destroy();
            missileFireCooldown_ = nullptr;
        }
    }

    void Ship::CreateBody()
    {
        // RS_MN.RenderParallelepiped* draws GetPosition() as the box's CENTER (its
        // built-in cube mesh spans [-0.5,0.5]), but CollisionArea::BodiesOverlap
        // treats a Body's position as the MIN CORNER ([pos, pos+size]). Offset by
        // -size/2 so the actual collision AABB lines up with what is rendered.
        CosmicEngine::Body *body = new CosmicEngine::Body(
            this,
            -GetSize() * 0.5f,
            GetSize(),
            CALLBACK_COLLISION_EVENT(OnBodyCollision));

        BOD_MN.Add(body);
        bodyId_ = body->GetID();
    }

    void Ship::OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side)
    {
        (void)other;
        (void)side;
        // Intentional no-op: ramming damage is owned by EnemyGrunt and pickup
        // collection is owned by Pickup -- each collision is resolved on exactly
        // one side of the interacting pair.
    }

    void Ship::init()
    {
        UpdateRailTransform(0.0f);
        assistedAimDirection_ = GetRawAimDirection();
    }

    void Ship::UpdateSteeringInput(float dt)
    {
        const float rawLateralAxis = INP_MN.GetActionAxis(kActionSteerLeft, kActionSteerRight);
        const float rawVerticalAxis = INP_MN.GetActionAxis(kActionSteerDown, kActionSteerUp);

        // Position within the tube still integrates the RAW axis, so lateral/vertical
        // movement speed stays exactly as responsive as before.
        lateralOffset_ = glm::clamp(lateralOffset_ + rawLateralAxis * steeringSpeed_ * dt, -lateralOffsetMax_, lateralOffsetMax_);
        verticalOffset_ = glm::clamp(verticalOffset_ + rawVerticalAxis * steeringSpeed_ * dt, -verticalOffsetMax_, verticalOffsetMax_);

        // lastLateralInput_/lastVerticalInput_ drive yaw/pitch/roll in UpdateRailTransform
        // AND GetAimDirection() (reticle + bullets). On keyboard, GetActionAxis() snaps
        // instantly between -1/0/+1 on every key press/release -- using that raw value
        // directly made the ship's orientation (and aim) jump instantly instead of
        // turning smoothly. Exponentially smoothing it here fixes both at once.
        constexpr float kInputSmoothRate = 7.0f;
        const float alpha = glm::clamp(kInputSmoothRate * dt, 0.0f, 1.0f);
        lastLateralInput_ = glm::mix(lastLateralInput_, rawLateralAxis, alpha);
        lastVerticalInput_ = glm::mix(lastVerticalInput_, rawVerticalAxis, alpha);
    }

    void Ship::UpdateBrakeStateMachine(float dt)
    {
        switch (brakeState_)
        {
        case BrakeState::Idle:
            currentSpeed_ = cruiseSpeed_;
            if (INP_MN.IsActionPressed(kActionBrake, CosmicEngine::KeyDown))
            {
                brakeState_ = BrakeState::Braking;
                brakeStateTimer_->SetTargetTime(2.0);
                brakeStateTimer_->reset();
                brakeStateTimer_->Play();
            }
            break;

        case BrakeState::Braking:
        {
            currentSpeed_ = glm::mix(currentSpeed_, brakeSpeed_, 1.0f - std::exp(-6.0f * dt));

            const bool timedOut = brakeStateTimer_->IsTrigger();
            const bool released = !INP_MN.IsActionPressed(kActionBrake, CosmicEngine::KeyRelease);
            if (timedOut || released)
            {
                brakeState_ = BrakeState::Recovering;
                brakeStateTimer_->SetTargetTime(1.2);
                brakeStateTimer_->reset();
                brakeStateTimer_->Play();
            }
            break;
        }

        case BrakeState::Recovering:
            currentSpeed_ = glm::mix(currentSpeed_, cruiseSpeed_, 1.0f - std::exp(-2.5f * dt));
            if (brakeStateTimer_->IsTrigger())
            {
                currentSpeed_ = cruiseSpeed_;
                brakeState_ = BrakeState::Cooldown;
                brakeStateTimer_->SetTargetTime(10.0);
                brakeStateTimer_->reset();
                brakeStateTimer_->Play();
            }
            break;

        case BrakeState::Cooldown:
            currentSpeed_ = cruiseSpeed_;
            if (brakeStateTimer_->IsTrigger())
            {
                brakeState_ = BrakeState::Idle;
            }
            break;
        }
    }

    void Ship::UpdateRailTransform(float dt)
    {
        if (!rail_)
        {
            return;
        }

        if (autoAdvanceEnabled_)
        {
            distanceTraveled_ = glm::clamp(distanceTraveled_ + currentSpeed_ * dt, 0.0f, rail_->GetTotalLength());
        }
        currentFrame_ = rail_->GetFrameAtDistance(distanceTraveled_);

        SetPosition(currentFrame_.position + currentFrame_.right * lateralOffset_ + currentFrame_.up * verticalOffset_);

        // Derive Euler yaw/pitch from the rail's forward vector, then layer steering
        // input on top. Assumes the model's local forward axis is (0,0,-1); if a real
        // model faces backward once loaded, add a 180deg yaw offset here (see nota de
        // integracion #6 del plan). Yaw AND pitch (not just roll) shift with steering
        // input so the nose actually turns into dodges, not just
        // banking -- which is also what GetAimDirection()/bullets/the reticle follow.
        const glm::vec3 f = currentFrame_.forward;
        const float horizLen = std::sqrt(f.x * f.x + f.z * f.z);
        const float yawDeg = glm::degrees(std::atan2(-f.x, -f.z)) + lastLateralInput_ * kMaxSteerYawDeg;
        const float pitchDeg = glm::degrees(std::atan2(f.y, horizLen)) - lastVerticalInput_ * kMaxSteerPitchDeg;
        const float rollDeg = -lastLateralInput_ * kMaxBankDeg;

        SetRotation(glm::vec3(pitchDeg, yawDeg, rollDeg));
    }

    void Ship::UpdateWeapons(float dt)
    {
        (void)dt;

        const bool fireHeld = INP_MN.IsActionPressed(kActionFirePrimary, CosmicEngine::KeyRelease);
        if (fireHeld && ammo_ > 0 && primaryFireCooldown_->IsFinished())
        {
            FireBullet();
            --ammo_;
            primaryFireCooldown_->SetTargetTime(0.12);
            primaryFireCooldown_->reset();
            primaryFireCooldown_->Play();
        }

        const bool missilePressed = INP_MN.IsActionPressed(kActionFireMissile, CosmicEngine::KeyDown);
        if (missilePressed && missiles_ > 0 && missileFireCooldown_->IsFinished())
        {
            FireMissile();
            --missiles_;
            missileFireCooldown_->SetTargetTime(0.8);
            missileFireCooldown_->reset();
            missileFireCooldown_->Play();
        }
    }

    void Ship::FireBullet()
    {
        const glm::vec3 spawnPos = GetNoseWorldPosition();
        OBJ_MN.Add(new Bullet(spawnPos, GetAimDirection()));
        AUD_MN.PlayAt(NABAZU_SFX_SHOOT_KEY, spawnPos, CosmicEngine::SoundSpace::World3D, 0.45f, false, true);
    }

    void Ship::FireMissile()
    {
        const glm::vec3 spawnPos = GetHardpointWorldPosition(nextHardpointIsLeft_);
        nextHardpointIsLeft_ = !nextHardpointIsLeft_;
        OBJ_MN.Add(new Missile(spawnPos, GetAimDirection()));
        AUD_MN.PlayAt(NABAZU_SFX_BOMB_KEY, spawnPos, CosmicEngine::SoundSpace::World3D, 0.7f, false, true);
    }

    void Ship::update(float deltaTime)
    {
        elapsedTime_ += deltaTime;

        UpdateSteeringInput(deltaTime);
        UpdateBrakeStateMachine(deltaTime);
        UpdateRailTransform(deltaTime);
        // Must run after UpdateRailTransform (it reads the rotation set there) and
        // before UpdateWeapons, so a shot fired this tick already uses this tick's
        // assisted direction rather than a one-tick-stale one.
        UpdateAimLock(deltaTime);
        UpdateWeapons(deltaTime);
    }

    void Ship::UpdateAimLock(float dt)
    {
        // Aim assist. Real 3D "am I pointing at an enemy" test -- NOT a screen-space
        // one: for each enemy, find the closest point on the aim ray to it and measure
        // the perpendicular miss distance. Inside the cone, the aim is bent toward the
        // target, turning near-misses into hits without ever moving the ship itself.
        //
        // The cone widens with distance (perpendicular tolerance grows with projected
        // length) so it is a true angular cone, and adds the enemy's own radius so
        // bigger enemies are proportionally easier to hit.
        constexpr float kAssistConeSlope = 0.13f; // ~7.4 degree half-angle
        constexpr float kAimMaxRange = 260.0f;
        // Miss fraction (0 = dead center, 1 = at the cone edge) at which assist starts
        // fading out. Inside this the pull is full, so a well-aimed shot always lands.
        // Tuned against a standalone sim of this math vs. EnemyGrunt's real hitbox:
        // 0.60 rescues aim errors up to ~5 degrees while still fading to zero by the
        // cone edge, so the aim is never yanked toward something you clearly missed.
        constexpr float kAssistFullPullError = 0.60f;
        constexpr float kLockSmoothRate = 10.0f;
        // Angular error alone is scale-free, so a distant enemy that happens to sit
        // nearer the crosshair beats a big, close one you are obviously engaging --
        // and the bullet then sails past the close threat entirely. Adding distance
        // to the score biases selection toward what is actually near.
        constexpr float kDistanceBias = 0.75f;

        const glm::vec3 rayOrigin = GetPosition();
        const glm::vec3 rawAim = GetRawAimDirection();

        CosmicEngine::Object *bestTarget = nullptr;
        float bestNormalizedError = 0.0f;
        float bestScore = 0.0f;

        for (auto *object : OBJ_MN.FindByClassName(EnemyGrunt::StaticClassName()))
        {
            if (!object)
            {
                continue;
            }

            const glm::vec3 toEnemy = object->GetPosition() - rayOrigin;
            const float projLength = glm::dot(toEnemy, rawAim);
            if (projLength <= 0.0f || projLength > kAimMaxRange)
            {
                continue;
            }

            const glm::vec3 closestPointOnRay = rayOrigin + rawAim * projLength;
            const float perpDist = glm::length(object->GetPosition() - closestPointOnRay);
            const float threshold = object->GetSize().x * 0.5f + projLength * kAssistConeSlope;
            if (perpDist > threshold)
            {
                continue;
            }

            // Score blends how centred the target is with how close it is, so a
            // well-aimed near enemy wins over a marginally better-aimed far one.
            const float normalizedError = perpDist / threshold;
            const float score = normalizedError + (projLength / kAimMaxRange) * kDistanceBias;
            if (!bestTarget || score < bestScore)
            {
                bestTarget = object;
                bestScore = score;
                bestNormalizedError = normalizedError;
            }
        }

        float desiredStrength = 0.0f;
        if (bestTarget)
        {
            const glm::vec3 toTarget = bestTarget->GetPosition() - rayOrigin;
            const float distance = glm::length(toTarget);
            if (distance > 1e-4f)
            {
                lastAssistTargetDir_ = toTarget / distance;
                desiredStrength = 1.0f - glm::smoothstep(kAssistFullPullError, 1.0f, bestNormalizedError);
            }
        }

        const float alpha = glm::clamp(kLockSmoothRate * dt, 0.0f, 1.0f);
        aimLockStrength_ = glm::mix(aimLockStrength_, desiredStrength, alpha);

        // Smoothing the STRENGTH (rather than the direction itself) keeps the aim
        // perfectly responsive when no target is in the cone: with strength 0 the aim
        // is exactly the raw nose direction, with zero lag added to normal flying.
        if (aimLockStrength_ > 1e-3f)
        {
            const glm::vec3 blended = glm::mix(rawAim, lastAssistTargetDir_, aimLockStrength_);
            assistedAimDirection_ = (glm::length(blended) > 1e-4f) ? glm::normalize(blended) : rawAim;
        }
        else
        {
            assistedAimDirection_ = rawAim;
        }
    }

    void Ship::draw() const
    {
        // Visual length is larger than the 3-unit collision cube so the fighter reads
        // properly at the chase-cam distance; the hitbox is deliberately left at
        // GetSize() (see CreateBody) rather than grown to match the new art.
        // World size of the model's largest dimension (wingspan for RedFighter).
        constexpr float kPlayerVisualSize = 19.5f; // 25% smaller than 26.0 (hitbox unchanged)
        const glm::vec3 modelRotation = GetRotation() + glm::vec3(0.0f, kModelYawOffsetDeg, 0.0f);
        DrawShipModel(GetPlayerModel(), GetPosition(), kPlayerVisualSize, modelRotation);

        DrawAimReticle();
    }

    void Ship::DrawAimReticle() const
    {
        // Crosshair mesh (see BuildUnitCrosshairVertices) instead of a plain cube;
        // shrinks and shifts to a hot color as aimLockStrength_ rises (computed each
        // tick in UpdateAimLock from the real 3D aim ray, not screen space).
        constexpr float kReticleLockedScale = 0.5f;

        const glm::vec3 reticlePos = GetPosition() + GetAimDirection() * kReticleDistance;
        const float sizeScale = glm::mix(1.0f, kReticleLockedScale, aimLockStrength_);
        CrosshairRenderer::GetInstance().Draw(
            reticlePos,
            glm::vec3(kReticleSize * sizeScale),
            glm::vec3(0.95f, 0.98f, 1.0f),
            aimLockStrength_,
            elapsedTime_);
    }

    void Ship::TakeDamage(int amount)
    {
        health_ = std::max(0, health_ - amount);
    }

    void Ship::HealPercent(float fraction)
    {
        const int restored = static_cast<int>(std::round(static_cast<float>(maxHealth_) * fraction));
        health_ = std::min(maxHealth_, health_ + std::max(1, restored));
    }

    void Ship::AddAmmo(int amount)
    {
        ammo_ = std::min(maxAmmo_, ammo_ + amount);
    }

    void Ship::AddMissiles(int amount)
    {
        missiles_ = std::min(maxMissiles_, missiles_ + amount);
    }

    float Ship::GetDistanceTraveled() const { return distanceTraveled_; }
    float Ship::GetLateralOffset() const { return lateralOffset_; }
    RailFrame Ship::GetCurrentRailFrame() const { return currentFrame_; }

    glm::vec3 Ship::GetNoseWorldPosition() const
    {
        return GetPosition() + RotateLocalOffset(noseOffsetLocal_, GetRotation());
    }

    glm::vec3 Ship::GetHardpointWorldPosition(bool leftWing) const
    {
        const glm::vec3 &offset = wingOffsetLocal_[leftWing ? 0 : 1];
        return GetPosition() + RotateLocalOffset(offset, GetRotation());
    }

    glm::vec3 Ship::GetRawAimDirection() const
    {
        return RotateLocalOffset(glm::vec3(0.0f, 0.0f, -1.0f), GetRotation());
    }

    glm::vec3 Ship::GetAimDirection() const
    {
        return assistedAimDirection_;
    }

    BrakeState Ship::GetBrakeState() const { return brakeState_; }
    float Ship::GetBrakeTimerElapsed() const { return brakeStateTimer_ ? static_cast<float>(brakeStateTimer_->GetElapsedTime()) : 0.0f; }
    float Ship::GetBrakeTimerTarget() const { return brakeStateTimer_ ? static_cast<float>(brakeStateTimer_->GetTargetTime()) : 1.0f; }

    int Ship::GetHealth() const { return health_; }
    int Ship::GetMaxHealth() const { return maxHealth_; }
    int Ship::GetAmmo() const { return ammo_; }
    int Ship::GetMaxAmmo() const { return maxAmmo_; }
    int Ship::GetMissiles() const { return missiles_; }
    int Ship::GetMaxMissiles() const { return maxMissiles_; }
    void Ship::SetAutoAdvanceEnabled(bool enabled) { autoAdvanceEnabled_ = enabled; }

    void Ship::NudgeRailDistance(float delta)
    {
        if (rail_)
        {
            distanceTraveled_ = glm::clamp(distanceTraveled_ + delta, 0.0f, rail_->GetTotalLength());
        }
    }

    float Ship::GetCurrentSpeed() const { return currentSpeed_; }
    float Ship::GetCruiseSpeed() const { return cruiseSpeed_; }
    float Ship::GetAimLockStrength() const { return aimLockStrength_; }
}
