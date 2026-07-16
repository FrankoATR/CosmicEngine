#include "torch_light_object.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include LIGHTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER

#include <algorithm>
#include <cmath>

namespace PuzzleRPG
{
    TorchLightObject::TorchLightObject(int id, Cell cell)
        : CosmicEngine::Object("TorchLight",
                               CellToWorld(cell),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               100),
          objectId(id),
          cell(cell)
    {
        // Reduce light radius to ~30% by increasing linear/quadratic falloff
        light = new CosmicEngine::Light(
            FlameCenter(),
            glm::vec2(0.08f, 0.08f),
            glm::vec2(0.55f, 0.55f),
            glm::vec2(0.18f, 0.18f),
            1.0f,
            0.06f,    // increased linear term (~3.33x)
            0.0089f,  // increased quadratic term (~11.1x)
            32.0f,
            true,
            CosmicEngine::LightMobility::Static);
        LGT_MN.Add(light);
        UpdateLight();

        spriteVisual.SetAnimatedClip(PUZZLERPG_CLIP_TORCH_IDLE, GetID());
    }

    TorchLightObject::~TorchLightObject()
    {
        ReleaseLight();
    }

    glm::vec2 TorchLightObject::FlameCenter() const
    {
        const glm::vec2 position = GetPosition();
        return position + glm::vec2(GetSize().x * 0.5f, GetSize().y * 0.32f);
    }

    void TorchLightObject::UpdateLight()
    {
        if (!light) return;
        light->SetPosition(FlameCenter());
        light->SetAmbientLight(glm::vec2(0.08f + 0.04f * flicker));
        light->SetDiffuseLight(glm::vec2(0.45f + 0.35f * flicker));
        light->SetSpecularLight(glm::vec2(0.12f + 0.08f * flicker));
    }

    void TorchLightObject::ReleaseLight()
    {
        if (!light) return;
        const int lightId = light->GetID();
        light = nullptr;
        LGT_MN.Remove(lightId);
    }

    void TorchLightObject::update(float deltaTime)
    {
        animationTime += deltaTime;
        const float pulse = 0.5f + 0.5f * std::sin(animationTime * 2.2f);
        const float flutter = 0.5f + 0.5f * std::sin(animationTime * 6.1f + 1.3f);
        flicker = std::clamp(0.72f + pulse * 0.18f + flutter * 0.10f, 0.0f, 1.0f);
        UpdateLight();
    }

    void TorchLightObject::draw() const
    {
        const glm::vec2 position = GetPosition();
        const glm::vec2 size = GetSize();

        // El sprite de la antorcha es de 16x32 (ancho x alto), es decir media celda
        // de ancho por una celda de alto. Se dibuja centrado horizontalmente.
        const glm::vec2 spriteSize(size.x * 0.5f, size.y);
        const glm::vec2 spritePosition = position + glm::vec2((size.x - spriteSize.x) * 0.5f, 0.0f);

        bool drewSprite = spriteVisual.Draw(spritePosition, spriteSize);
        if (!drewSprite && IsPuzzleDebugVisualsEnabled())
        {
            const glm::vec2 stemMin = position + glm::vec2(size.x * 0.43f, size.y * 0.35f);
            const glm::vec2 stemMax = position + glm::vec2(size.x * 0.57f, size.y * 0.96f);
            RS_MN.RenderRectangle(stemMin, stemMax, glm::vec2(0.0f), glm::vec2(0.0f), glm::vec3(0.33f, 0.20f, 0.08f), 1.0f, 1.0f, true);

            const float flameShift = std::sin(animationTime * 4.0f) * 0.8f;
            const glm::vec2 flameCenter = position + glm::vec2(size.x * 0.5f + flameShift, size.y * 0.28f);
            const glm::vec2 outerHalf(4.0f, 5.0f);
            const glm::vec2 innerHalf(2.5f, 3.0f);
            RS_MN.RenderRectangle(flameCenter - outerHalf, flameCenter + outerHalf,
                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.96f, 0.56f, 0.12f), 0.75f + 0.20f * flicker, 1.0f, true);
            RS_MN.RenderRectangle(flameCenter - innerHalf, flameCenter + innerHalf,
                                  glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(1.0f, 0.88f, 0.42f), 0.85f + 0.10f * flicker, 1.0f, true);
        }
    }
}
