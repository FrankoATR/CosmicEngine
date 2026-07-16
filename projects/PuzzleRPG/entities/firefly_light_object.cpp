#include "firefly_light_object.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include LIGHTMANAGER_HEADER
#include RESOURCEMANAGER_HEADER

#include <algorithm>
#include <cmath>

namespace PuzzleRPG
{
    FireflyLightObject::FireflyLightObject(int id, Cell anchorCell, float seed)
        : CosmicEngine::Object("FireflyLight",
                               CellToWorld(anchorCell),
                               glm::vec2(3.0f, 3.0f),
                               0.0f,
                               100),
          objectId(id),
          anchorCell(anchorCell),
          seed(seed)
    {
        anchorPosition = CellToWorld(anchorCell) + glm::vec2(kTileSize * 0.5f, kTileSize * 0.45f);
        currentCenter = anchorPosition;
        SetPosition(currentCenter - GetSize() * 0.5f);

        // Compact accent light: keep the local punch, but reduce the radius so
        // the effect stays concentrated around the insect.
        light = new CosmicEngine::Light(
            currentCenter,
            glm::vec2(255.0f, 255.0f),
            glm::vec2(255.0f, 255.0f),
            glm::vec2(255.0f, 255.0f),
            1.0f,
            0.16f,
            0.10f,
            128.0f,
            true,
            CosmicEngine::LightMobility::Dynamic);
        LGT_MN.Add(light);
        UpdateLight();
    }

    FireflyLightObject::~FireflyLightObject()
    {
        ReleaseLight();
    }

    void FireflyLightObject::ReleaseLight()
    {
        if (!light) return;
        const int lightId = light->GetID();
        light = nullptr;
        LGT_MN.Remove(lightId);
    }

    void FireflyLightObject::UpdateMotion()
    {
        const float x = std::sin(animationTime * 0.63f + seed) * 6.0f
                      + std::sin(animationTime * 1.17f + seed * 1.7f) * 2.0f;
        const float y = std::cos(animationTime * 0.51f + seed * 1.3f) * 5.0f
                      + std::sin(animationTime * 0.91f + seed * 2.1f) * 2.5f;

        currentCenter = anchorPosition + glm::vec2(x, y);
        SetPosition(currentCenter - GetSize() * 0.5f);

        const float primary = 0.5f + 0.5f * std::sin(animationTime * 0.82f + seed * 3.2f);
        const float secondary = 0.5f + 0.5f * std::sin(animationTime * 0.27f + seed * 1.4f);
        flicker = std::clamp(0.35f + primary * 0.45f + secondary * 0.20f, 0.0f, 1.0f);
    }

    void FireflyLightObject::UpdateLight()
    {
        if (!light) return;
        light->SetPosition(currentCenter);
        // Keep the flicker, but give the firefly enough energy to brighten
        // the nearby terrain instead of reading only as a helper glow sprite.
        light->SetAmbientLight(glm::vec2(0.05f + 0.05f * flicker));
        light->SetDiffuseLight(glm::vec2(0.30f + 0.35f * flicker));
        light->SetSpecularLight(glm::vec2(0.10f + 0.08f * flicker));
    }

    void FireflyLightObject::update(float deltaTime)
    {
        animationTime += deltaTime;
        UpdateMotion();
        UpdateLight();
    }

    void FireflyLightObject::draw() const
    {
        const glm::vec2 center = GetPosition() + GetSize() * 0.5f;
        const glm::vec2 glowHalf(0.75f + 0.30f * flicker, 0.75f + 0.30f * flicker);
        const glm::vec2 coreHalf(0.375f + 0.10f * flicker, 0.375f + 0.10f * flicker);
        RS_MN.RenderRectangle(center - glowHalf, center + glowHalf,
                              glm::vec2(0.0f), glm::vec2(0.0f),
                      glm::vec3(0.72f, 0.96f, 0.36f), 0.22f + 0.16f * flicker, 1.0f, true);
        RS_MN.RenderRectangle(center - coreHalf, center + coreHalf,
                              glm::vec2(0.0f), glm::vec2(0.0f),
                              glm::vec3(0.94f, 1.0f, 0.58f), 0.85f + 0.15f * flicker, 1.0f, true);
    }
}
