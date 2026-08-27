#ifndef NABAZU_SHIP_HPP
#define NABAZU_SHIP_HPP

#include "../systems/rail_path.hpp"

#include <CosmicEngine/models/body/body.hpp>
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/timer/timer.hpp>

#include <glm/glm.hpp>

#include <string>

namespace NaBazu
{
    enum class BrakeState
    {
        Idle,
        Braking,
        Recovering,
        Cooldown
    };

    // Player ship. NEVER uses Object::SetVelocity -- it computes position/rotation
    // every tick directly from the rail via SetPosition/SetRotation, avoiding the
    // engine's automatic velocity integration entirely (unlike Bullet/Missile/Pickup).
    class Ship : public CosmicEngine::Object
    {
    public:
        static const std::string &StaticClassName();

        explicit Ship(RailPath *rail, glm::vec3 initialOffset = glm::vec3(0.0f));
        ~Ship() override;

        void init() override;
        void update(float deltaTime) override;
        void draw() const override;

        void TakeDamage(int amount);
        void AddAmmo(int amount);
        void AddMissiles(int amount);
        // fraction is 0..1 of MAX health (0.25 = restore a quarter of the full bar),
        // so healing stays meaningful regardless of how maxHealth_ is later tuned.
        void HealPercent(float fraction);

        float GetDistanceTraveled() const;
        float GetLateralOffset() const;
        RailFrame GetCurrentRailFrame() const;
        glm::vec3 GetNoseWorldPosition() const;
        glm::vec3 GetHardpointWorldPosition(bool leftWing) const;
        // Where shots actually go: the nose direction (GetRawAimDirection) after the
        // aim assist has bent it toward a nearby target. Bullets, missiles AND the
        // reticle all use this, so what you see is always where you shoot.
        glm::vec3 GetAimDirection() const;
        // Unassisted local-forward direction after full rotation (yaw+pitch+roll).
        // Deliberately NOT the rail's tangent (currentFrame_.forward), since the nose
        // visibly turns into steering input (see UpdateRailTransform). Target search
        // must use THIS, never GetAimDirection(), or the assist would feed on its own
        // output and latch onto a target it could never release.
        glm::vec3 GetRawAimDirection() const;

        BrakeState GetBrakeState() const;
        float GetBrakeTimerElapsed() const;
        float GetBrakeTimerTarget() const;

        int GetHealth() const;
        int GetMaxHealth() const;
        int GetAmmo() const;
        int GetMaxAmmo() const;
        int GetMissiles() const;
        int GetMaxMissiles() const;
        float GetCurrentSpeed() const;
        float GetCruiseSpeed() const;
        // 0..1 strength of the aim assist right now; the HUD surfaces it so the
        // player can see when a target is being tracked.
        float GetAimLockStrength() const;

        // --- TEMPORARY test-mode hooks (see systems/test_mode.hpp) ---------------
        // Disables the automatic forward flight so the rail position can be stepped
        // by hand, and nudges that position directly.
        void SetAutoAdvanceEnabled(bool enabled);
        void NudgeRailDistance(float delta);

    private:
        void UpdateSteeringInput(float dt);
        void UpdateBrakeStateMachine(float dt);
        void UpdateRailTransform(float dt);
        void UpdateWeapons(float dt);
        void UpdateAimLock(float dt);
        void FireBullet();
        void FireMissile();
        void CreateBody();
        void OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side);
        void DrawAimReticle() const;

        RailPath *rail_;
        int bodyId_;
        bool autoAdvanceEnabled_;
        float elapsedTime_;
        // Smoothed 0..1 strength of the aim assist: 0 = no target in the assist cone
        // (aim is exactly the raw nose direction), 1 = target well centered (aim is
        // pulled fully onto it). Eased over a few ticks so engaging/releasing a target
        // animates instead of popping. Also drives the reticle's shrink/color, so the
        // reticle honestly shows how much help you are getting. See UpdateAimLock.
        float aimLockStrength_;
        // Direction toward the most recent assist target, retained while
        // aimLockStrength_ decays back to 0 after the target is lost/destroyed.
        glm::vec3 lastAssistTargetDir_;
        // Final assisted aim direction returned by GetAimDirection().
        glm::vec3 assistedAimDirection_;

        float distanceTraveled_;
        float cruiseSpeed_;
        float brakeSpeed_;
        float currentSpeed_;

        float lateralOffset_;
        float verticalOffset_;
        float lateralOffsetMax_;
        float verticalOffsetMax_;
        float steeringSpeed_;
        float lastLateralInput_;
        float lastVerticalInput_;

        RailFrame currentFrame_;
        glm::vec3 noseOffsetLocal_;
        glm::vec3 wingOffsetLocal_[2];

        BrakeState brakeState_;
        CosmicEngine::Timer *brakeStateTimer_;

        int health_;
        int maxHealth_;
        int ammo_;
        int maxAmmo_;
        int missiles_;
        int maxMissiles_;
        CosmicEngine::Timer *primaryFireCooldown_;
        CosmicEngine::Timer *missileFireCooldown_;
        bool nextHardpointIsLeft_;
    };
}

#endif
