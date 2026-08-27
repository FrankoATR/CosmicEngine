#include "level_director.hpp"

#include "nabazu_level_data.hpp"
#include "../utilities/random_utils.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kRailLookAheadDistance = 450.0f; // > WaveSpawner's own lookahead (220) + camera lookahead (24)
    constexpr float kWaveLookAhead = 220.0f;          // matches WaveSpawner's default lookAheadDistance
    constexpr float kDifficultyRampDistance = 4000.0f;
}

namespace NaBazu
{
    LevelDirector::LevelDirector(RailPath *rail, WaveSpawner *waveSpawner, float initialWaveTriggerDistance)
        : rail_(rail),
          waveSpawner_(waveSpawner),
          committedWaypoints_(GetCorridorWaypoints()),
          nextWaveTriggerDistance_(initialWaveTriggerDistance),
          proceduralStartDistance_(initialWaveTriggerDistance)
    {
    }

    void LevelDirector::Update(float shipDistanceTraveled)
    {
        if (!rail_ || !waveSpawner_)
        {
            return;
        }

        while (rail_->GetTotalLength() - shipDistanceTraveled < kRailLookAheadDistance)
        {
            AppendRandomSegment();
        }

        MaybeGenerateNextWave(shipDistanceTraveled);
    }

    void LevelDirector::AppendRandomSegment()
    {
        if (committedWaypoints_.size() < 2)
        {
            return;
        }

        const auto &library = GetRailSegmentLibrary();
        const RailSegmentTemplate &chosen = library[static_cast<std::size_t>(NaBazu::RandomRange(0.0f, static_cast<float>(library.size()) - 0.01f))];

        const glm::vec3 &tail = committedWaypoints_.back();
        const glm::vec3 &beforeTail = committedWaypoints_[committedWaypoints_.size() - 2];
        glm::vec3 exitDir = tail - beforeTail;
        exitDir.y = 0.0f;
        if (glm::length(exitDir) < 1e-4f)
        {
            exitDir = glm::vec3(0.0f, 0.0f, -1.0f);
        }
        exitDir = glm::normalize(exitDir);

        // Same atan2(-x,-z) convention Ship::UpdateRailTransform uses for yaw, solved
        // here for the angle that rotates local (0,0,-1) onto exitDir (yaw-only, so
        // templates authored with mild Y deltas stay mild after rotation).
        const float theta = std::atan2(-exitDir.x, -exitDir.z);
        const glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(0.0f, 1.0f, 0.0f));

        std::vector<glm::vec3> newWaypoints;
        newWaypoints.reserve(chosen.relativePoints.size());
        for (const glm::vec3 &relativePoint : chosen.relativePoints)
        {
            const glm::vec3 worldPoint = tail + glm::vec3(rotation * glm::vec4(relativePoint, 0.0f));
            newWaypoints.push_back(worldPoint);
            committedWaypoints_.push_back(worldPoint);
        }

        rail_->AppendWaypoints(newWaypoints);
    }

    float LevelDirector::DifficultyForDistance(float distance) const
    {
        return glm::clamp((distance - proceduralStartDistance_) / kDifficultyRampDistance, 0.0f, 1.0f);
    }

    void LevelDirector::MaybeGenerateNextWave(float shipDistanceTraveled)
    {
        while (shipDistanceTraveled + kWaveLookAhead > nextWaveTriggerDistance_)
        {
            waveSpawner_->AddWave(BuildProceduralWave(nextWaveTriggerDistance_));

            const float difficulty = DifficultyForDistance(nextWaveTriggerDistance_);
            const float interval = std::max(90.0f, NaBazu::RandomRange(140.0f, 220.0f) - difficulty * 40.0f);
            nextWaveTriggerDistance_ += interval;
        }
    }

    WaveDefinition LevelDirector::BuildProceduralWave(float triggerDistance) const
    {
        const float difficulty = DifficultyForDistance(triggerDistance);

        WaveDefinition wave;
        wave.triggerDistance = triggerDistance;
        wave.enemyCount = 4 + static_cast<int>(difficulty * 10.0f) + static_cast<int>(NaBazu::RandomRange(0.0f, 2.99f));
        wave.formation = static_cast<FormationType>(static_cast<int>(NaBazu::RandomRange(0.0f, 5.99f)));
        wave.lateralSpread = NaBazu::RandomRange(22.0f, 42.0f); // widened for the larger ship models
        wave.verticalOffset = NaBazu::RandomRange(-4.0f, 6.0f);
        wave.distanceSpacing = NaBazu::RandomRange(14.0f, 26.0f); // scaled with the larger craft
        wave.amplitude = NaBazu::RandomRange(14.0f, 30.0f);
        wave.frequency = NaBazu::RandomRange(0.5f, 2.0f);

        const float shotChance = 0.25f + difficulty * 0.35f;
        wave.shotPattern = (NaBazu::RandomFloat01() < shotChance)
                                ? (NaBazu::RandomFloat01() < 0.5f ? EnemyShotPattern::Forward : EnemyShotPattern::AimAtPlayer)
                                : EnemyShotPattern::None;
        wave.shotSpeed = NaBazu::RandomRange(45.0f, 70.0f) + difficulty * 20.0f;
        wave.enemyHealth = 15 + static_cast<int>(difficulty * 18.0f); // scaled down alongside the 30->18 base health cut
        wave.enemySize = 12.0f;

        return wave;
    }
}
