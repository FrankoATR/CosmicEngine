#ifndef NABAZU_WAVE_SPAWNER_HPP
#define NABAZU_WAVE_SPAWNER_HPP

#include "rail_path.hpp"

#include <cstddef>
#include <vector>

namespace NaBazu
{
    // Some enemies fire straight back down the corridor (cheap/predictable), others
    // aim directly at the ship's current position each shot (slower cadence, precise).
    enum class EnemyShotPattern
    {
        None,
        Forward,
        AimAtPlayer
    };

    // Formation shapes enemies within a wave are arranged into, each a small closed-
    // form function of the enemy's index within the wave -- see WaveSpawner::SpawnWave.
    enum class FormationType
    {
        Line,
        VFormation,
        SineWave,
        Ring,
        Spiral,
        Grid
    };

    struct WaveDefinition
    {
        float triggerDistance;
        int enemyCount;
        FormationType formation = FormationType::Line;
        float lateralSpread = 26.0f;   // Line/V/Grid: half-width. Ring/Spiral: radius.
        float verticalOffset = 2.0f;   // baseline vertical offset from the rail center
        float distanceSpacing = 5.0f;  // Line/V/SineWave/Spiral: spacing along the rail
        float amplitude = 8.0f;        // SineWave/Spiral: oscillation amplitude
        float frequency = 1.0f;        // SineWave/Spiral: cycles across the group
        EnemyShotPattern shotPattern = EnemyShotPattern::None;
        float shotSpeed = 55.0f;
        int enemyHealth = 18; // lowered from 30 -- with the default 10-damage Bullet, ~2 hits kills a grunt
        // Index into GetEnemyModels(). -1 (the default) means "roll one at random when
        // this wave spawns", so every enemy in a wave shares a model while different
        // waves look different.
        int modelIndex = -1;
        float enemySize = 12.0f; // logical size == hitbox; visual is 1.4x this
    };

    // Plain (non-Object) system, ticked once per frame from GameplayScene::update().
    // Spawns enemy waves in mathematically-defined formations as the ship's distance
    // traveled along the rail approaches each wave's trigger distance, positioning
    // enemies relative to the rail's own frame rather than free-floating coordinates.
    class WaveSpawner
    {
    public:
        WaveSpawner(RailPath *rail, std::vector<WaveDefinition> waves, float lookAheadDistance = 220.0f);

        void Update(float shipDistanceTraveled);

        // Appends a procedurally-generated wave to the tail of the queue. Callers
        // (LevelDirector) are responsible for keeping triggerDistance increasing.
        void AddWave(WaveDefinition wave);

        std::size_t GetRemainingWaveCount() const;
        float GetLastQueuedTriggerDistance() const;

    private:
        void SpawnWave(const WaveDefinition &wave);
        // Returns (lateral, vertical) offset from the wave's base RailFrame for
        // enemy `index` of `wave.enemyCount`, and writes the along-rail distance
        // offset (added to wave.triggerDistance) to outDistanceOffset.
        glm::vec2 ComputeFormationOffset(const WaveDefinition &wave, int index, float &outDistanceOffset) const;

        RailPath *rail_;
        std::vector<WaveDefinition> waves_;
        std::size_t nextWaveIndex_;
        float lookAheadDistance_;
    };
}

#endif
