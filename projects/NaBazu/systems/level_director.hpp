#ifndef NABAZU_LEVEL_DIRECTOR_HPP
#define NABAZU_LEVEL_DIRECTOR_HPP

#include "rail_path.hpp"
#include "wave_spawner.hpp"

#include <vector>

namespace NaBazu
{
    // Makes the level infinite: appends randomly-chosen rail segments (from
    // GetRailSegmentLibrary()) well ahead of the ship, and keeps queuing
    // procedurally-generated, difficulty-scaled waves into a WaveSpawner once the
    // curated opening table (GetWaveTable()) runs out. Ticked once per frame from
    // GameplayScene::update(); does not own rail_/waveSpawner_.
    class LevelDirector
    {
    public:
        LevelDirector(RailPath *rail, WaveSpawner *waveSpawner, float initialWaveTriggerDistance);

        void Update(float shipDistanceTraveled);

    private:
        void AppendRandomSegment();
        void MaybeGenerateNextWave(float shipDistanceTraveled);
        WaveDefinition BuildProceduralWave(float triggerDistance) const;
        float DifficultyForDistance(float distance) const;

        RailPath *rail_;
        WaveSpawner *waveSpawner_;
        std::vector<glm::vec3> committedWaypoints_;
        float nextWaveTriggerDistance_;
        float proceduralStartDistance_;
    };
}

#endif
