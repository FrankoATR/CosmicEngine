#include "wall_block_object.hpp"

#include "../systems/debug_text.hpp"
#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

#include <string>

namespace PuzzleRPG
{
    WallBlockObject::WallBlockObject(int id, int sheet, int row, int col, Cell topLeft, int span)
        : CosmicEngine::Object("WallBlock",
                               CellToWorld(topLeft),
                               glm::vec2(kTileSize * static_cast<float>(span < 1 ? 1 : span),
                                         kTileSize * static_cast<float>(span < 1 ? 1 : span)),
                               0.0f,
                               1),
          objectId(id),
          sheet(sheet),
          row(row),
          col(col),
          cell(topLeft),
          span(span < 1 ? 1 : span)
    {
        SetColor(glm::vec3(0.22f, 0.18f, 0.15f));
    }

    void WallBlockObject::draw() const
    {
        glm::vec2 position = GetPosition();
        glm::vec2 size     = GetSize();

        bool drewSprite = DrawSheetCell(SheetKeyForSheetId(sheet), row, col, position, size);
        if (!drewSprite && IsPuzzleDebugVisualsEnabled())
        {
            RS_MN.RenderRectangle(position,
                                  position + size,
                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                  GetColor(), 1.0f, 1.0f, true);
            RS_MN.RenderRectangle(position,
                                  position + size,
                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.05f), 1.0f, 1.5f, false);
        }

        RenderDebugTextBlock(position + size * 0.5f,
                             {
                                 {"Wall" + std::to_string(span) + "x" + std::to_string(span),
                                  glm::vec3(0.95f, 0.85f, 0.55f)},
                                 {"#" + std::to_string(objectId), glm::vec3(1.0f, 0.92f, 0.45f)}
                             });
    }
}
