#include "pressure_plate.hpp"

#include "../systems/class_ids.hpp"
#include "../systems/debug_text.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER
#include LIGHTMANAGER_HEADER

namespace PuzzleRPG
{
    PressurePlate::PressurePlate(int id, int textureId, Cell cell, int activator, std::vector<int> targets,
                                 int requiredItemTextureId)
        : CosmicEngine::Object("PressurePlate",
                               CellToWorld(cell),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               1),
          objectId(id),
          textureId(textureId),
          cell(cell),
          activator(activator),
          active(false),
          requiredItemTextureId(requiredItemTextureId),
          targets(std::move(targets))
    {
        if (activator == Plate_SpecificItem)
            SetColor(glm::vec3(0.20f, 0.60f, 0.45f));
        else
            SetColor(glm::vec3(0.45f, 0.35f, 0.55f));

        spriteVisual.SetStaticClip(ClipForPressurePlate(active, textureId));

        if (textureId == DefaultTextureId::kReceptacle)
        {
            const glm::vec2 center = GetPosition() + GetSize() * 0.5f;
            light = new CosmicEngine::Light(
                center,
                glm::vec2(0.08f, 0.08f),
                glm::vec2(0.55f, 0.55f),
                glm::vec2(0.18f, 0.18f),
                1.0f,
                0.06f,
                0.0089f,
                32.0f,
                false,
                CosmicEngine::LightMobility::Static);
            light->SetColorTint(glm::vec3(0.30f, 0.55f, 1.0f));
            LGT_MN.Add(light);
        }
    }

    PressurePlate::~PressurePlate()
    {
        ReleaseLight();
    }

    void PressurePlate::ReleaseLight()
    {
        if (!light) return;
        const int lightId = light->GetID();
        light = nullptr;
        LGT_MN.Remove(lightId);
    }

    void PressurePlate::SetActive(bool value)
    {
        active = value;
        spriteVisual.SetStaticClip(ClipForPressurePlate(active, textureId));
        if (light) light->SetVisible(active);
    }

    void PressurePlate::draw() const
    {
        glm::vec2 position = GetPosition();
        glm::vec2 size     = GetSize();

        bool drewSprite = spriteVisual.Draw(position, size);
        if (!drewSprite && IsPuzzleDebugVisualsEnabled())
        {
            // Suaviza el rectangulo de la placa hacia dentro de la celda.
            glm::vec2 inner = position + glm::vec2(kTileSize * 0.10f, kTileSize * 0.10f);
            glm::vec2 outer = inner + glm::vec2(kTileSize * 0.80f, kTileSize * 0.80f);
            glm::vec3 color = active ? glm::vec3(0.85f, 0.45f, 0.85f) : GetColor();

            RS_MN.RenderRectangle(inner, outer, glm::vec2(0.0f), glm::vec2(0.0f), color, 1.0f, 1.0f, true);
            RS_MN.RenderRectangle(inner, outer, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec3(0.05f), 0.9f, 1.5f, false);
        }

        std::vector<std::pair<std::string, glm::vec3>> lines =
        {
            {"Placa",                                   glm::vec3(1.0f)},
            {"C" + std::to_string(Class_PressurePlate), glm::vec3(0.75f, 0.95f, 1.0f)},
            {"T" + std::to_string(textureId),           glm::vec3(0.65f, 1.0f, 0.60f)},
            {"#" + std::to_string(objectId),            glm::vec3(1.0f, 0.92f, 0.45f)}
        };
        if (activator == Plate_SpecificItem && requiredItemTextureId > 0)
        {
            lines.push_back({"R" + std::to_string(requiredItemTextureId), glm::vec3(1.0f, 0.72f, 0.58f)});
        }
        if (active)
        {
            lines.push_back({"ON", glm::vec3(0.45f, 1.0f, 0.55f)});
        }
        RenderDebugTextBlock(position + size * 0.5f, lines, 0.24f, 8.5f, 58.0f);
    }
}
