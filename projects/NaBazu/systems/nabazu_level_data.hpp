#ifndef NABAZU_LEVEL_DATA_HPP
#define NABAZU_LEVEL_DATA_HPP

#include "wave_spawner.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace NaBazu
{
    // Waypoints for the curated opening stretch (~1650 units). The rail curves gently
    // in X (S-curve) and Y (mild elevation change) while making steady forward progress
    // along -Z. RailPath turns these into a smooth Catmull-Rom curve; past this point
    // LevelDirector appends procedurally-chosen segments from GetRailSegmentLibrary()
    // forever (Fase 2 -- the level is infinite, see plan seccion 4).
    inline const std::vector<glm::vec3> &GetCorridorWaypoints()
    {
        static const std::vector<glm::vec3> waypoints = {
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(20.0f, 5.0f, -120.0f),
            glm::vec3(55.0f, 10.0f, -250.0f),
            glm::vec3(60.0f, 5.0f, -380.0f),
            glm::vec3(30.0f, -10.0f, -500.0f),
            glm::vec3(-20.0f, -15.0f, -620.0f),
            glm::vec3(-55.0f, -5.0f, -750.0f),
            glm::vec3(-50.0f, 15.0f, -880.0f),
            glm::vec3(-10.0f, 25.0f, -1000.0f),
            glm::vec3(40.0f, 20.0f, -1120.0f),
            glm::vec3(70.0f, 5.0f, -1250.0f),
            glm::vec3(50.0f, -10.0f, -1380.0f),
            glm::vec3(0.0f, -5.0f, -1500.0f),
            glm::vec3(0.0f, 0.0f, -1620.0f)
        };
        return waypoints;
    }

    // Curated waves for the opening stretch, escalating in count/formation/shot
    // pattern. LevelDirector takes over with procedurally-generated waves once these
    // run out (nextWaveTriggerDistance_ starts at 1550, see level_director.cpp).
    inline const std::vector<WaveDefinition> &GetWaveTable()
    {
        static const std::vector<WaveDefinition> waves = {
            WaveDefinition{.triggerDistance = 150.0f, .enemyCount = 3, .formation = FormationType::Line,
                           .lateralSpread = 20.0f, .verticalOffset = 3.0f, .distanceSpacing = 18.0f},
            WaveDefinition{.triggerDistance = 300.0f, .enemyCount = 4, .formation = FormationType::VFormation,
                           .lateralSpread = 24.0f, .verticalOffset = 2.0f, .distanceSpacing = 18.0f},
            WaveDefinition{.triggerDistance = 480.0f, .enemyCount = 5, .formation = FormationType::SineWave,
                           .lateralSpread = 24.0f, .verticalOffset = 4.0f, .distanceSpacing = 15.0f,
                           .amplitude = 10.0f, .frequency = 1.5f},
            WaveDefinition{.triggerDistance = 650.0f, .enemyCount = 5, .formation = FormationType::Line,
                           .lateralSpread = 28.0f, .verticalOffset = 2.0f, .distanceSpacing = 15.0f,
                           .shotPattern = EnemyShotPattern::Forward, .shotSpeed = 50.0f},
            WaveDefinition{.triggerDistance = 820.0f, .enemyCount = 8, .formation = FormationType::Ring,
                           .lateralSpread = 32.0f, .verticalOffset = 3.0f},
            WaveDefinition{.triggerDistance = 1000.0f, .enemyCount = 6, .formation = FormationType::VFormation,
                           .lateralSpread = 32.0f, .verticalOffset = 2.0f, .distanceSpacing = 13.5f,
                           .shotPattern = EnemyShotPattern::AimAtPlayer, .shotSpeed = 55.0f},
            WaveDefinition{.triggerDistance = 1180.0f, .enemyCount = 10, .formation = FormationType::Spiral,
                           .lateralSpread = 28.0f, .verticalOffset = 2.0f, .distanceSpacing = 12.0f},
            WaveDefinition{.triggerDistance = 1350.0f, .enemyCount = 9, .formation = FormationType::Grid,
                           .lateralSpread = 36.0f, .verticalOffset = 0.0f,
                           .shotPattern = EnemyShotPattern::Forward, .shotSpeed = 60.0f}
        };
        return waves;
    }

    // Relative waypoint chains for procedural rail extension. Authored assuming the
    // segment is entered heading along local -Z; LevelDirector rotates each template
    // (yaw only) to match the current path's exit heading before appending it -- see
    // systems/level_director.cpp.
    struct RailSegmentTemplate
    {
        std::vector<glm::vec3> relativePoints;
    };

    inline const std::vector<RailSegmentTemplate> &GetRailSegmentLibrary()
    {
        static const std::vector<RailSegmentTemplate> segments = {
            // Straight
            {{glm::vec3(5.0f, 2.0f, -110.0f), glm::vec3(-4.0f, -3.0f, -230.0f), glm::vec3(3.0f, 1.0f, -350.0f)}},
            // GentleLeftCurve
            {{glm::vec3(-25.0f, 4.0f, -110.0f), glm::vec3(-55.0f, 6.0f, -210.0f), glm::vec3(-70.0f, 2.0f, -320.0f)}},
            // GentleRightCurve
            {{glm::vec3(25.0f, -3.0f, -110.0f), glm::vec3(55.0f, -6.0f, -210.0f), glm::vec3(70.0f, -1.0f, -320.0f)}},
            // SCurve
            {{glm::vec3(35.0f, 3.0f, -110.0f), glm::vec3(45.0f, 8.0f, -230.0f), glm::vec3(0.0f, 5.0f, -330.0f),
              glm::vec3(-40.0f, 0.0f, -430.0f), glm::vec3(-45.0f, -4.0f, -540.0f)}},
            // ClimbDive
            {{glm::vec3(10.0f, 30.0f, -120.0f), glm::vec3(-5.0f, 45.0f, -250.0f), glm::vec3(0.0f, 10.0f, -380.0f),
              glm::vec3(5.0f, -20.0f, -500.0f)}},
            // SharpHook
            {{glm::vec3(30.0f, 2.0f, -80.0f), glm::vec3(70.0f, 4.0f, -110.0f), glm::vec3(95.0f, 6.0f, -60.0f),
              glm::vec3(90.0f, 8.0f, 10.0f)}},
            // Corkscrew
            {{glm::vec3(30.0f, 20.0f, -90.0f), glm::vec3(0.0f, 35.0f, -180.0f), glm::vec3(-30.0f, 20.0f, -270.0f),
              glm::vec3(0.0f, 5.0f, -360.0f), glm::vec3(0.0f, 5.0f, -450.0f)}}
        };
        return segments;
    }
}

#endif
