#include "pickup.hpp"

#include "ship.hpp"
#include "../systems/glow_mesh_renderer.hpp"
#include "../utilities/nabazu_asset_defines.hpp"

#include "../systems/test_mode.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include AUDIOMANAGER_HEADER
#include BODYMANAGER_HEADER
#include RESOURCEMANAGER_HEADER
#include TIMERMANAGER_HEADER

namespace NaBazu
{
    const std::string &Pickup::StaticClassName()
    {
        static const std::string className = "Pickup";
        return className;
    }

    Pickup::Pickup(glm::vec3 position, PickupKind kind, int amount)
        : Object(StaticClassName(), position, glm::vec3(1.5f)),
          kind_(kind),
          amount_(amount),
          bodyId_(-1),
          spinSpeedDeg_(420.0f), // fast spin so the star silhouette catches the eye
          lifetimeTimer_(nullptr)
    {
        switch (kind_)
        {
        case PickupKind::Ammo:
            SetColor(glm::vec3(0.25f, 0.95f, 0.35f));
            break;
        case PickupKind::Missile:
            SetColor(glm::vec3(0.75f, 0.25f, 0.95f));
            break;
        case PickupKind::Health:
            SetColor(glm::vec3(1.0f, 0.18f, 0.22f));
            // The ring reads best rotating gently, unlike the fast-spinning stars.
            spinSpeedDeg_ = 110.0f;
            break;
        }

        lifetimeTimer_ = new CosmicEngine::Timer(8.0, false, false);
        CosmicEngine::TimerManager::GetInstance().Add(lifetimeTimer_);

        CreateBody();
    }

    Pickup::~Pickup()
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

    void Pickup::CreateBody()
    {
        // See Ship::CreateBody: rendering treats GetPosition() as the box center,
        // collision treats a Body's position as its min corner -- offset by -size/2
        // so the two agree. Hitbox is intentionally larger than the visual mesh
        // (kHitboxScale) so pickups are easy to sweep up while flying past.
        constexpr float kHitboxScale = 2.0f;
        const glm::vec3 hitboxSize = GetSize() * kHitboxScale;
        CosmicEngine::Body *body = new CosmicEngine::Body(
            this,
            -hitboxSize * 0.5f,
            hitboxSize,
            CALLBACK_COLLISION_EVENT(OnBodyCollision));

        BOD_MN.Add(body);
        bodyId_ = body->GetID();
    }

    void Pickup::OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side)
    {
        (void)side;

        if (!other || other->GetClassName() != Ship::StaticClassName())
        {
            return;
        }

        Ship *ship = static_cast<Ship *>(other);
        switch (kind_)
        {
        case PickupKind::Ammo:
            ship->AddAmmo(amount_);
            break;
        case PickupKind::Missile:
            ship->AddMissiles(amount_);
            break;
        case PickupKind::Health:
            // amount_ is a PERCENTAGE of max health for this kind, not a flat count.
            ship->HealPercent(static_cast<float>(amount_) * 0.01f);
            break;
        }

        AUD_MN.PlayAt(NABAZU_SFX_PICKUP_KEY, GetPosition(), CosmicEngine::SoundSpace::World3D, 0.7f, false, true);
        Destroy();
    }

    void Pickup::update(float deltaTime)
    {
        // TEMPORARY test mode: hold position (and skip lifetime expiry) while the
        // world is frozen for collision inspection.
        if (IsWorldFrozen())
        {
            return;
        }

        // Object exposes no public angular-velocity setter, so the spin is applied
        // manually here (same pattern as the engine's SandboxHammerObject3D).
        SetRotation(GetRotation() + glm::vec3(0.0f, spinSpeedDeg_ * deltaTime, 0.0f));

        if (lifetimeTimer_ && lifetimeTimer_->IsFinished())
        {
            Destroy();
        }
    }

    void Pickup::draw() const
    {
        const float elapsedTime = lifetimeTimer_ ? static_cast<float>(lifetimeTimer_->GetElapsedTime()) : 0.0f;

        if (kind_ == PickupKind::Health)
        {
            RingPickupRenderer::GetInstance().Draw(
                GetPosition(), GetSize(), GetRotation(), GetColor(), elapsedTime);
            return;
        }

        // 3-pointed star for ammo, 5-pointed for missile ammo, both driven by the
        // shared iridescent star shader (tinted per kind via GetColor()).
        const int pointCount = (kind_ == PickupKind::Ammo) ? 3 : 5;
        StarPickupRenderer::GetInstance().Draw(
            pointCount, GetPosition(), GetSize(), GetRotation(), GetColor(), elapsedTime);
    }
}
