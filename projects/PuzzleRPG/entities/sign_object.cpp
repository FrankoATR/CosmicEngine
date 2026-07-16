#include "sign_object.hpp"

#include "../utilities/puzzle_sprite_support.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER

namespace PuzzleRPG
{
    SignObject::SignObject(int id, Cell cell)
        : CosmicEngine::Object("Sign",
                               CellToWorld(cell),
                               glm::vec2(kTileSize, kTileSize),
                               0.0f,
                               100),
          objectId(id),
          cell(cell)
    {
        spriteVisual.SetStaticClip(PUZZLERPG_CLIP_SIGN_IDLE);
    }

    void SignObject::update(float deltaTime)
    {
        (void)deltaTime;
    }

    void SignObject::draw() const
    {
        const glm::vec2 position = GetPosition();
        const glm::vec2 size = GetSize();

        bool drewSprite = spriteVisual.Draw(position, size);
        if (!drewSprite && IsPuzzleDebugVisualsEnabled())
        {
            const glm::vec2 postMin = position + glm::vec2(size.x * 0.44f, size.y * 0.40f);
            const glm::vec2 postMax = position + glm::vec2(size.x * 0.56f, size.y * 0.95f);
            RS_MN.RenderRectangle(postMin, postMax, glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.39f, 0.24f, 0.12f), 1.0f, 1.0f, true);

            const glm::vec2 boardMin = position + glm::vec2(size.x * 0.20f, size.y * 0.12f);
            const glm::vec2 boardMax = position + glm::vec2(size.x * 0.80f, size.y * 0.48f);
            RS_MN.RenderRectangle(boardMin, boardMax, glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.92f, 0.84f, 0.62f), 1.0f, 1.0f, true);
            RS_MN.RenderRectangle(boardMin, boardMax, glm::vec2(0.0f), glm::vec2(0.0f),
                                  glm::vec3(0.55f, 0.32f, 0.12f), 1.0f, 1.0f, false);
        }
    }
}
