#ifndef NABAZU_ENEMY_GRUNT_HPP
#define NABAZU_ENEMY_GRUNT_HPP

#include "../systems/wave_spawner.hpp"

#include <CosmicEngine/models/body/body.hpp>
#include <CosmicEngine/models/object/object.hpp>
#include <CosmicEngine/models/timer/timer.hpp>

#include <glm/glm.hpp>

#include <string>

namespace NaBazu
{
    // MVP enemy type: rail-anchored bobbing motion, no chase AI (explicitly out of
    // scope). Owns ship-ramming damage and the death -> pickup-drop roll, since
    // collision reactions must live on exactly one side of each interacting pair.
    // Fase 2: also fires (Forward or AimAtPlayer, see EnemyShotPattern) and despawns
    // itself once the ship has traveled well past its spawn point (railDistance_),
    // required now that the level is infinite and nothing else culls stale enemies.
    class EnemyGrunt : public CosmicEngine::Object
    {
    public:
        static const std::string &StaticClassName();

        explicit EnemyGrunt(glm::vec3 spawnPosition, float railDistance,
                             EnemyShotPattern shotPattern = EnemyShotPattern::None,
                             float shotSpeed = 55.0f, int health = 18, float size = 12.0f,
                             int modelIndex = 0);
        ~EnemyGrunt() override;

        void update(float deltaTime) override;
        void draw() const override;

        void TakeDamage(int amount);
        float GetRailDistance() const;

        // Called once per frame by GameplayScene with the ship's current distance;
        // self-destroys once left far enough behind (see nota #4e del plan).
        void MaybeDespawnBehind(float shipDistanceTraveled, float despawnMargin);

    private:
        void CreateBody();
        void OnBodyCollision(CosmicEngine::Object *other, CosmicEngine::BodyCollisionSide side);
        void OnDeath();
        void UpdateFiring(float deltaTime);

        glm::vec3 spawnOrigin_;
        float railDistance_;
        float bobPhase_;
        float bobAmplitude_;
        float bobSpeed_;
        int health_;
        int maxHealth_;
        int ramDamage_;
        int bodyId_;
        int modelIndex_; // index into GetEnemyModels(); assigned per wave by WaveSpawner
        // Yaw (degrees) resolved ONCE at spawn so the craft is turned toward the
        // oncoming player. Fixed at spawn rather than re-aimed per frame: these are
        // static gun emplacements, and a constantly-turning model would read as
        // homing AI that does not exist.
        float facingYawDeg_;
        bool dead_;

        EnemyShotPattern shotPattern_;
        float shotSpeed_;
        CosmicEngine::Timer *fireCooldown_;
    };
}

#endif
