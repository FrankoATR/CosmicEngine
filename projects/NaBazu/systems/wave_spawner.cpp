#include "wave_spawner.hpp"

#include "../entities/enemy_grunt.hpp"
#include "../utilities/nabazu_models.hpp"
#include "../utilities/random_utils.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include OBJECTMANAGER_HEADER

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kTwoPi = 6.28318530718f;
}

namespace NaBazu
{
    WaveSpawner::WaveSpawner(RailPath *rail, std::vector<WaveDefinition> waves, float lookAheadDistance)
        : rail_(rail),
          waves_(std::move(waves)),
          nextWaveIndex_(0),
          lookAheadDistance_(lookAheadDistance)
    {
    }

    void WaveSpawner::Update(float shipDistanceTraveled)
    {
        while (nextWaveIndex_ < waves_.size() &&
               shipDistanceTraveled + lookAheadDistance_ >= waves_[nextWaveIndex_].triggerDistance)
        {
            SpawnWave(waves_[nextWaveIndex_]);
            ++nextWaveIndex_;
        }
    }

    void WaveSpawner::AddWave(WaveDefinition wave)
    {
        waves_.push_back(wave);
    }

    std::size_t WaveSpawner::GetRemainingWaveCount() const
    {
        return waves_.size() - nextWaveIndex_;
    }

    float WaveSpawner::GetLastQueuedTriggerDistance() const
    {
        return waves_.empty() ? 0.0f : waves_.back().triggerDistance;
    }

    glm::vec2 WaveSpawner::ComputeFormationOffset(const WaveDefinition &wave, int index, float &outDistanceOffset) const
    {
        const int count = wave.enemyCount;
        const float i = static_cast<float>(index);

        switch (wave.formation)
        {
        case FormationType::VFormation:
        {
            const float center = (count > 1) ? (static_cast<float>(count - 1) * 0.5f) : 0.0f;
            const float lateral = (center > 0.0f) ? wave.lateralSpread * (i - center) / center : 0.0f;
            const float depthFromVertex = std::abs(i - center);
            outDistanceOffset = depthFromVertex * wave.distanceSpacing;
            return glm::vec2(lateral, wave.verticalOffset + depthFromVertex * 0.6f);
        }
        case FormationType::SineWave:
        {
            const float lateral = wave.amplitude * std::sin(kTwoPi * wave.frequency * i / static_cast<float>(count > 0 ? count : 1));
            outDistanceOffset = i * wave.distanceSpacing;
            return glm::vec2(lateral, wave.verticalOffset);
        }
        case FormationType::Ring:
        {
            const float theta = kTwoPi * i / static_cast<float>(count > 0 ? count : 1);
            outDistanceOffset = 0.0f;
            return glm::vec2(wave.lateralSpread * std::cos(theta), wave.verticalOffset + wave.lateralSpread * std::sin(theta));
        }
        case FormationType::Spiral:
        {
            const float theta = kTwoPi * i / static_cast<float>(count > 0 ? count : 1) + 0.35f * i;
            outDistanceOffset = i * wave.distanceSpacing;
            return glm::vec2(wave.lateralSpread * std::cos(theta), wave.verticalOffset + wave.lateralSpread * std::sin(theta));
        }
        case FormationType::Grid:
        {
            const int cols = std::max(1, static_cast<int>(std::ceil(std::sqrt(static_cast<float>(count > 0 ? count : 1)))));
            const int col = index % cols;
            const int row = index / cols;
            const float lateral = (cols > 1) ? wave.lateralSpread * (static_cast<float>(col) / static_cast<float>(cols - 1) * 2.0f - 1.0f) : 0.0f;
            outDistanceOffset = 0.0f;
            return glm::vec2(lateral, wave.verticalOffset + static_cast<float>(row) * 4.0f);
        }
        case FormationType::Line:
        default:
        {
            const float alpha = (count > 1) ? i / static_cast<float>(count - 1) : 0.5f;
            const float lateral = glm::mix(-wave.lateralSpread, wave.lateralSpread, alpha);
            outDistanceOffset = i * wave.distanceSpacing;
            return glm::vec2(lateral, wave.verticalOffset);
        }
        }
    }

    void WaveSpawner::SpawnWave(const WaveDefinition &wave)
    {
        if (!rail_)
        {
            return;
        }

        // Resolved ONCE per wave, not per enemy, so a formation reads as a single
        // squadron of identical craft.
        const int modelCount = static_cast<int>(GetEnemyModels().size());
        const int waveModelIndex = (wave.modelIndex >= 0)
                                        ? wave.modelIndex
                                        : static_cast<int>(NaBazu::RandomRange(0.0f, static_cast<float>(modelCount))) % modelCount;

        for (int i = 0; i < wave.enemyCount; ++i)
        {
            float distanceOffset = 0.0f;
            const glm::vec2 offset = ComputeFormationOffset(wave, i, distanceOffset);
            const float enemyDistance = wave.triggerDistance + distanceOffset;
            const RailFrame frame = rail_->GetFrameAtDistance(enemyDistance);

            const glm::vec3 spawnPos = frame.position + frame.right * offset.x + frame.up * offset.y;
            OBJ_MN.Add(new EnemyGrunt(spawnPos, enemyDistance, wave.shotPattern, wave.shotSpeed, wave.enemyHealth, wave.enemySize, waveModelIndex));
        }
    }
}
