#include "decor_object.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

namespace PuzzleRPG
{
    DecorObject::DecorObject(int id, int spriteSheet, int spriteRow, int spriteCol, Cell cell)
        : CosmicEngine::Object("Decor",
                               CellToWorld(cell),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               100),
          objectId(id),
          cell(cell),
          spriteSheet(spriteSheet),
          spriteRow(spriteRow),
          spriteCol(spriteCol)
    {
    }

    void DecorObject::draw() const
    {
        const glm::vec2 position = GetPosition();
        const glm::vec2 size     = GetSize();

        const char *sheetKey = SheetKeyForSheetId(spriteSheet);
        bool drew = DrawSheetCell(sheetKey, spriteRow, spriteCol, position, size);

        if (!drew && IsPuzzleDebugVisualsEnabled())
        {
            RS_MN.RenderRectangle(position, position + size,
                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.55f, 0.40f, 0.70f), 0.55f, 1.0f, true);
            RS_MN.RenderRectangle(position, position + size,
                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.80f, 0.60f, 1.0f), 1.0f, 1.0f, false);
        }
    }
}
