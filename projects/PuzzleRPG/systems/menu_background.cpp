#include "menu_background.hpp"

#include "../utilities/paths.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include CAMERAMANAGER_HEADER
#include RESOURCEMANAGER_HEADER

#include <cmath>
#include <chrono>

namespace PuzzleRPG
{
    MenuBackground &MenuBackground::Get()
    {
        static MenuBackground instance;
        return instance;
    }

    MenuBackground::MenuBackground()
        : position(0.0f), size(0.0f), velocity(0.0f), changeTimer(0.0), speed(60.0f), rng(static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())), angleDist(0.0f, 2.0f * 3.14159265f)
    {
    }

    MenuBackground::~MenuBackground() {}

    void MenuBackground::LoadResources()
    {
        RS_MN.LoadTexture(textureKey, PUZZLERPG_MENU_BACKGROUND_PATH);
    }

    void MenuBackground::Init()
    {
        // initial position centered on camera focus
        glm::vec2 camFocus = CAM_MN.GetFocusPosition();
        glm::vec2 base = CAM_MN.GetBaseWindowSize();
        // ensure menu renders at UI/world default zoom so size calculation is stable
        CAM_MN.SetZoom(1.0f);
        size = base * 2.0f;
        position = camFocus;
        velocity = glm::vec2(0.0f);
        changeTimer = 0.0;
        ChooseNewDirection();
    }

    void MenuBackground::ChooseNewDirection()
    {
        float angle = angleDist(rng);
        velocity = glm::vec2(std::cos(angle), std::sin(angle)) * speed;
    }

    void MenuBackground::Update(double dt)
    {
        if (dt <= 0.0) return;
        changeTimer += dt;
        if (changeTimer >= 5.0)
        {
            changeTimer = 0.0;
            ChooseNewDirection();
        }

        // integrate movement
        position += velocity * static_cast<float>(dt);

        // constrain so camera view remains inside the background
        glm::vec2 camFocus = CAM_MN.GetFocusPosition();
        glm::vec2 camSize = CAM_MN.GetBaseWindowSize();
        glm::vec2 halfMargin = (size - camSize) * 0.5f;
        if (halfMargin.x < 0.0f) halfMargin.x = 0.0f;
        if (halfMargin.y < 0.0f) halfMargin.y = 0.0f;

        // clamp position so the camera focus is contained
        float minX = camFocus.x - halfMargin.x;
        float maxX = camFocus.x + halfMargin.x;
        float minY = camFocus.y - halfMargin.y;
        float maxY = camFocus.y + halfMargin.y;

        if (position.x < minX) { position.x = minX; velocity.x = -velocity.x * 0.5f; }
        if (position.x > maxX) { position.x = maxX; velocity.x = -velocity.x * 0.5f; }
        if (position.y < minY) { position.y = minY; velocity.y = -velocity.y * 0.5f; }
        if (position.y > maxY) { position.y = maxY; velocity.y = -velocity.y * 0.5f; }
    }

    void MenuBackground::Draw()
    {
        // draw the background texture centered at position with size
        glm::vec2 drawPos = position - size * 0.5f;
        // draw with 30% transparency (alpha = 0.7)
        RS_MN.Render2DSpriteUnlit(textureKey, drawPos, size, 0.0f, glm::vec3(1.0f), 0.7f, CosmicEngine::ViewType::Ortho);
    }

}
