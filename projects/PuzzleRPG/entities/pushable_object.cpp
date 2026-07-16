#include "pushable_object.hpp"

#include "../systems/class_ids.hpp"
#include "../systems/debug_text.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

namespace PuzzleRPG
{
    PushableObject::PushableObject(int id, int textureId, Cell cell)
        : CosmicEngine::Object("Pushable",
                               CellToWorld(cell),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               2),
          objectId(id),
          textureId(textureId),
          cell(cell),
          tweenFromCell(cell),
          tweenToCell(cell)
    {
        SetColor(glm::vec3(0.70f, 0.50f, 0.30f));
        spriteVisual.SetStaticClip(ClipForTextureId(textureId));
    }

    void PushableObject::update(float deltaTime)
    {
        if (!tweenActive) return;

        tweenProgress += deltaTime / tweenDuration;
        if (tweenProgress >= 1.0f)
        {
            tweenActive = false;
            tweenProgress = 1.0f;
            SetPosition(CellToWorld(tweenToCell));
            return;
        }

        glm::vec2 start = CellToWorld(tweenFromCell);
        glm::vec2 end = CellToWorld(tweenToCell);
        SetPosition(start + (end - start) * tweenProgress);
    }

    void PushableObject::SetCell(Cell newCell)
    {
        cell = newCell;
        tweenFromCell = newCell;
        tweenToCell = newCell;
        tweenActive = false;
        tweenProgress = 0.0f;
        SetPosition(CellToWorld(newCell));
    }

    void PushableObject::BeginMoveTo(Cell newCell, float durationSeconds)
    {
        tweenFromCell = cell;
        tweenToCell = newCell;
        tweenDuration = std::max(0.05f, durationSeconds);
        tweenProgress = 0.0f;
        tweenActive = true;
        cell = newCell;
    }

    void PushableObject::SetSpriteVariant(int sheet, int row, int col)
    {
        spriteSheet = sheet;
        spriteRow   = row;
        spriteCol   = col;
    }

    void PushableObject::draw() const
    {
        glm::vec2 position = GetPosition();
        glm::vec2 size     = GetSize();

        bool drewSprite = false;
        if (spriteSheet > 0)
        {
            drewSprite = DrawSheetCell(SheetKeyForSheetId(spriteSheet), spriteRow, spriteCol, position, size);
        }
        else
        {
            drewSprite = spriteVisual.Draw(position, size);
        }
        if (!drewSprite && IsPuzzleDebugVisualsEnabled())
        {
            RS_MN.RenderRectangle(position, position + size, glm::vec2(0.0f), glm::vec2(0.0f), GetColor(), 1.0f, 1.0f, true);
            RS_MN.RenderRectangle(position, position + size, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec3(0.20f, 0.12f, 0.06f), 1.0f, 1.5f, false);
        }

        RenderDebugTextBlock(position + size * 0.5f,
                             {
                                 {"Pushable",                           glm::vec3(1.0f)},
                                 {"C" + std::to_string(Class_Pushable), glm::vec3(0.75f, 0.95f, 1.0f)},
                                 {"T" + std::to_string(textureId),      glm::vec3(0.65f, 1.0f, 0.60f)},
                                 {"#" + std::to_string(objectId),       glm::vec3(1.0f, 0.92f, 0.45f)}
                             });
    }
}
