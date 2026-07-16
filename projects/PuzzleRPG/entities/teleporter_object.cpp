#include "teleporter_object.hpp"

#include "../systems/class_ids.hpp"
#include "../systems/debug_text.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include LIGHTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER

namespace PuzzleRPG
{
    TeleporterObject::TeleporterObject(int id, int textureId, Cell cell, int pairId)
        : CosmicEngine::Object("Teleporter",
                               CellToWorld(cell),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               1),
          objectId(id),
          textureId(textureId),
          cell(cell),
          pairId(pairId)
    {
        SetColor(glm::vec3(0.30f, 0.85f, 0.95f));
        spriteVisual.SetAnimatedClip(ClipForTextureId(textureId), GetID(), true);

        const glm::vec2 center = GetPosition() + GetSize() * 0.5f;
        light = new CosmicEngine::Light(
            center,
            glm::vec2(0.04f, 0.04f),
            glm::vec2(0.28f, 0.28f),
            glm::vec2(0.09f, 0.09f),
            1.0f,
            0.06f,
            0.0089f,
            32.0f,
            true,
            CosmicEngine::LightMobility::Static);
        light->SetColorTint(glm::vec3(0.40f, 0.80f, 1.0f));
        LGT_MN.Add(light);
    }

    TeleporterObject::~TeleporterObject()
    {
        ReleaseLight();
    }

    void TeleporterObject::ReleaseLight()
    {
        if (!light) return;
        const int lightId = light->GetID();
        light = nullptr;
        LGT_MN.Remove(lightId);
    }

    void TeleporterObject::draw() const
    {
        glm::vec2 position = GetPosition();

        bool drewSprite = spriteVisual.Draw(position, GetSize());
        if (!drewSprite && IsPuzzleDebugVisualsEnabled())
        {
            // Disco exterior.
            glm::vec2 outerA = position + glm::vec2(kTileSize * 0.08f, kTileSize * 0.08f);
            glm::vec2 outerB = position + glm::vec2(kTileSize * 0.92f, kTileSize * 0.92f);
            RS_MN.RenderRectangle(outerA, outerB, glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.10f, 0.30f, 0.55f), 0.95f, 1.0f, true);

            // Anillo intermedio.
            glm::vec2 ringA = position + glm::vec2(kTileSize * 0.18f, kTileSize * 0.18f);
            glm::vec2 ringB = position + glm::vec2(kTileSize * 0.82f, kTileSize * 0.82f);
            RS_MN.RenderRectangle(ringA, ringB, glm::vec2(0.0f), glm::vec2(0.0f),
                                  GetColor(), 1.0f, 1.0f, true);

            // Nucleo brillante.
            glm::vec2 coreA = position + glm::vec2(kTileSize * 0.32f, kTileSize * 0.32f);
            glm::vec2 coreB = position + glm::vec2(kTileSize * 0.68f, kTileSize * 0.68f);
            RS_MN.RenderRectangle(coreA, coreB, glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.95f, 0.98f, 1.0f), 1.0f, 1.0f, true);
        }

        RenderDebugTextBlock(position + glm::vec2(kTileSize * 0.5f, kTileSize * 0.5f),
                             {
                                 {"Teleporter",                         glm::vec3(1.0f)},
                                 {"C" + std::to_string(Class_Teleporter), glm::vec3(0.75f, 0.95f, 1.0f)},
                                 {"P" + std::to_string(pairId),           glm::vec3(0.55f, 0.90f, 1.0f)},
                                 {"#" + std::to_string(objectId),         glm::vec3(1.0f, 0.92f, 0.45f)}
                             });
    }
}
