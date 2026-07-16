#include "pickup_object.hpp"

#include "../systems/class_ids.hpp"
#include "../systems/debug_text.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

#include <cmath>

namespace PuzzleRPG
{
    PickupObject::PickupObject(int id, int textureId, Cell cell, std::string dialog)
        : CosmicEngine::Object("Pickup",
                               CellToWorld(cell),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               2),
          objectId(id),
          textureId(textureId),
          cell(cell),
          carried(false),
          dialog(std::move(dialog))
    {
        SetColor(glm::vec3(0.95f, 0.85f, 0.30f));
        spriteVisual.SetStaticClip(ClipForTextureId(textureId));
    }

    void PickupObject::SetCell(Cell newCell)
    {
        cell = newCell;
        SetPosition(CellToWorld(newCell));
    }

    void PickupObject::SetCarried(bool value)
    {
        carried = value;
        SetVisible(!value);
        if (value) onAltar = false;
    }

    void PickupObject::SetOnAltar(bool value)
    {
        onAltar = value;
        if (!value) levitateTime = 0.0f;
    }

    void PickupObject::update(float deltaTime)
    {
        if (onAltar) levitateTime += deltaTime;
    }

    void PickupObject::SetSpriteVariant(int sheet, int row, int col)
    {
        spriteSheet = sheet;
        spriteRow   = row;
        spriteCol   = col;
    }

    void PickupObject::draw() const
    {
        if (carried) return;

        glm::vec2 position = GetPosition();
        glm::vec2 size     = GetSize();

        if (onAltar)
        {
            const glm::vec2 fullSize = size;
            size = fullSize * 0.5f;
            const float levitation = std::sin(levitateTime * 2.0f) * (fullSize.y * 0.08f);
            const float hoverOffset = fullSize.y * 0.25f;
            position.x = position.x + (fullSize.x - size.x) * 0.5f;
            position.y = position.y + levitation - hoverOffset;
        }

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
            RS_MN.RenderRectangle(position,
                                  position + size,
                                  glm::vec2(0.0f),
                                  glm::vec2(0.0f),
                                  GetColor(),
                                  1.0f,
                                  1.0f,
                                  true);
        }

        RenderDebugTextBlock(position + size * 0.5f,
                             {
                                 {"Pickup",                            glm::vec3(1.0f)},
                                 {"C" + std::to_string(Class_Pickup), glm::vec3(0.75f, 0.95f, 1.0f)},
                                 {"T" + std::to_string(textureId),    glm::vec3(0.65f, 1.0f, 0.60f)},
                                 {"#" + std::to_string(objectId),     glm::vec3(1.0f, 0.92f, 0.45f)}
                             });
    }
}
