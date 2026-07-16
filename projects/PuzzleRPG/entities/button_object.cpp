#include "button_object.hpp"

#include "../systems/class_ids.hpp"
#include "../systems/debug_text.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

namespace PuzzleRPG
{
    ButtonObject::ButtonObject(int id, int textureId, Cell cell, int requiredFacing, std::vector<int> targets)
        : CosmicEngine::Object("Button",
                               CellToWorld(cell),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               2),
          objectId(id),
          textureId(textureId),
          cell(cell),
          requiredFacing(requiredFacing),
          active(false),
          targets(std::move(targets))
    {
        SetColor(glm::vec3(0.30f, 0.55f, 0.85f));
        spriteVisual.SetStaticClip(ClipForButton(active, textureId));
    }

    void ButtonObject::Toggle()
    {
        SetActive(!active);
    }

    void ButtonObject::SetActive(bool value)
    {
        active = value;
        spriteVisual.SetStaticClip(ClipForButton(active, textureId));
    }

    void ButtonObject::draw() const
    {
        glm::vec2 position = GetPosition();
        glm::vec2 size     = GetSize();

        bool drewSprite = spriteVisual.Draw(position, size);
        if (!drewSprite && IsPuzzleDebugVisualsEnabled())
        {
            glm::vec2 inner = position + glm::vec2(kTileSize * 0.20f, kTileSize * 0.20f);
            glm::vec2 outer = inner + glm::vec2(kTileSize * 0.60f, kTileSize * 0.60f);

            glm::vec3 color = active ? glm::vec3(0.30f, 0.95f, 0.55f) : GetColor();

            RS_MN.RenderRectangle(inner, outer, glm::vec2(0.0f), glm::vec2(0.0f), color, 1.0f, 1.0f, true);
            RS_MN.RenderRectangle(inner, outer, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec3(0.05f), 0.9f, 1.5f, false);
        }

        RenderDebugTextBlock(position + size * 0.5f,
                             {
                                 {"Boton",                               glm::vec3(1.0f)},
                                 {"C" + std::to_string(Class_Button),    glm::vec3(0.75f, 0.95f, 1.0f)},
                                 {"T" + std::to_string(textureId),       glm::vec3(0.65f, 1.0f, 0.60f)},
                                 {"#" + std::to_string(objectId),        glm::vec3(1.0f, 0.92f, 0.45f)},
                                 {active ? "ON" : "OFF",               active ? glm::vec3(0.45f, 1.0f, 0.55f) : glm::vec3(1.0f, 0.60f, 0.55f)}
                             });
    }
}
