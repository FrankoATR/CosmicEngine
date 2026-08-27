#include "obstacle_object.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER
#include BODYMANAGER_HEADER
#include <CosmicEngine/models/body/body.hpp>

namespace MiniScene
{
    ObstacleObject::ObstacleObject(glm::vec2 position)
        : CosmicEngine::Object("ObstacleObject", position, glm::vec2(64.0f), 0.0f, 1)
    {
    }

    void ObstacleObject::init()
    {
        CosmicEngine::Body *body = new CosmicEngine::Body(
            this, glm::vec2(0.0f), GetSize(), nullptr);
        BOD_MN.Add(body);
    }

    void ObstacleObject::draw() const
    {
        RS_MN.Render2DSpriteUnlit("obstacle", GetPosition(), GetSize());
    }
}
