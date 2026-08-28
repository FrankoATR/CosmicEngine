#pragma once
#include <glm/glm.hpp>

// Shared state between the benchmark scene and its objects.
namespace Bench
{
#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    using Vec = glm::vec3;
#else
    using Vec = glm::vec2;
#endif

    extern long long collisionCount;   // incremented by body callbacks while countCollisions is true
    extern bool countCollisions;
    extern Vec areaMin;                // movement bounds (inclusive min corner)
    extern Vec areaMax;                // movement bounds (exclusive max corner)
}
