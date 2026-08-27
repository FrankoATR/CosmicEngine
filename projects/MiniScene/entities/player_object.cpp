#include "player_object.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER
#include AUDIOMANAGER_HEADER
#include BODYMANAGER_HEADER
#include <CosmicEngine/managers/input/input_manager.hpp>
#include <GLFW/glfw3.h>

namespace MiniScene
{
    PlayerObject::PlayerObject(glm::vec2 position)
        : CosmicEngine::Object("PlayerObject", position, glm::vec2(64.0f), 0.0f, 1)
    {
    }

    void PlayerObject::init()
    {
        CosmicEngine::Body *body = new CosmicEngine::Body(
            this, glm::vec2(0.0f), GetSize(),
            CALLBACK_COLLISION_EVENT(OnBodyCollision));
        BOD_MN.Add(body);
    }

    void PlayerObject::update(float deltaTime)
    {
        const float speed = 300.0f;
        glm::vec2 pos = GetPosition();
        if (INP_MN.IsKeyPressed(GLFW_KEY_LEFT, CosmicEngine::KeyRelease))  pos.x -= speed * deltaTime;
        if (INP_MN.IsKeyPressed(GLFW_KEY_RIGHT, CosmicEngine::KeyRelease)) pos.x += speed * deltaTime;
        if (INP_MN.IsKeyPressed(GLFW_KEY_UP, CosmicEngine::KeyRelease))    pos.y -= speed * deltaTime;
        if (INP_MN.IsKeyPressed(GLFW_KEY_DOWN, CosmicEngine::KeyRelease))  pos.y += speed * deltaTime;
        SetPosition(pos);

        if (touching && !wasTouching)
            AUD_MN.Play("hit");
        wasTouching = touching;
        touching = false;
    }

    void PlayerObject::draw() const
    {
        RS_MN.Render2DSpriteUnlit("player", GetPosition(), GetSize());
    }

    void PlayerObject::OnBodyCollision(CosmicEngine::Object *, CosmicEngine::BodyCollisionSide)
    {
        touching = true;
    }
}
