#include "bench_state.hpp"

namespace Bench
{
    long long collisionCount = 0;
    bool countCollisions = false;
#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    Vec areaMin(-640.0f, -360.0f, -360.0f);
    Vec areaMax(640.0f, 360.0f, 360.0f);
#else
    Vec areaMin(-640.0f, -360.0f);
    Vec areaMax(640.0f, 360.0f);
#endif
}
