#include "collision_block.hpp"

namespace PuzzleRPG
{
    CollisionBlock::CollisionBlock(int objectId, Cell cell)
        : CosmicEngine::Object("CollisionBlock",
                               CellToWorld(cell),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               static_cast<short>(1)),
          objectId(objectId),
          cell(cell)
    {
    }
}
