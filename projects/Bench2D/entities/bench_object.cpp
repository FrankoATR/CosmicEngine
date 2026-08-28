#include "bench_object.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include RESOURCEMANAGER_HEADER
#include BODYMANAGER_HEADER

namespace Bench
{
#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    BenchObject::BenchObject(Vec position, Vec size, Vec velocity, bool withBody, bool withSprite)
        : CosmicEngine::Object("BenchObject", position, size, glm::vec3(0.0f)),
          velocity(velocity), withBody(withBody), withSprite(withSprite)
    {
    }
#else
    BenchObject::BenchObject(Vec position, Vec size, Vec velocity, bool withBody, bool withSprite)
        : CosmicEngine::Object("BenchObject", position, size, 0.0f, 0),
          velocity(velocity), withBody(withBody), withSprite(withSprite)
    {
    }
#endif

    void BenchObject::init()
    {
        if (withBody)
        {
            CosmicEngine::Body *body = new CosmicEngine::Body(
                this, Vec(0.0f), GetSize(),
                CALLBACK_COLLISION_EVENT(OnCollision));
            BOD_MN.Add(body);
        }
    }

    void BenchObject::update(float deltaTime)
    {
        Vec pos = GetPosition();
        const Vec size = GetSize();
        pos += velocity * deltaTime;

        // Reflect on the area bounds so that the population density stays constant.
        for (int axis = 0; axis < static_cast<int>(Vec::length()); ++axis)
        {
            if (pos[axis] < areaMin[axis])
            {
                pos[axis] = areaMin[axis];
                velocity[axis] = -velocity[axis];
            }
            else if (pos[axis] + size[axis] > areaMax[axis])
            {
                pos[axis] = areaMax[axis] - size[axis];
                velocity[axis] = -velocity[axis];
            }
        }
        SetPosition(pos);
    }

    void BenchObject::draw() const
    {
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        if (withSprite)
        {
            RS_MN.Render2DSpriteUnlit("sprite", GetPosition(), GetSize());
        }
#endif
    }

    void BenchObject::OnCollision(CosmicEngine::Object *, CosmicEngine::BodyCollisionSide)
    {
        if (countCollisions)
        {
            ++collisionCount;
            ++ownCallbacks;
        }
    }
}
