#include "CollisionArea.hpp"

#include "../models/body/body.hpp"

#include <algorithm>

namespace CosmicEngine
{
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    CollisionArea::CollisionArea(glm::vec2 position, glm::vec2 size, CollisionType type)
        : areaType(type), areaPosition(position), areaSize(size)
    {
    }

    glm::vec2 CollisionArea::GetPosition() const
    {
        return areaPosition;
    }

    glm::vec2 CollisionArea::GetSize() const
    {
        return areaSize;
    }

    bool CollisionArea::BodiesOverlap(Body *body1, Body *body2) const
    {
        return body1->GetPosition().x < body2->GetPosition().x + body2->GetSize().x &&
               body1->GetPosition().x + body1->GetSize().x > body2->GetPosition().x &&
               body1->GetPosition().y < body2->GetPosition().y + body2->GetSize().y &&
               body1->GetPosition().y + body1->GetSize().y > body2->GetPosition().y;
    }

    BodyCollisionSide CollisionArea::GetCollisionSide(Body *body1, Body *body2) const
    {
        float deltaLeft = (body2->GetPosition().x + body2->GetSize().x) - body1->GetPosition().x;
        float deltaRight = (body1->GetPosition().x + body1->GetSize().x) - body2->GetPosition().x;
        float deltaTop = (body2->GetPosition().y + body2->GetSize().y) - body1->GetPosition().y;
        float deltaBottom = (body1->GetPosition().y + body1->GetSize().y) - body2->GetPosition().y;

        float minDelta = std::min({deltaLeft, deltaRight, deltaTop, deltaBottom});

        if (minDelta == deltaLeft) return BodyCollisionSide::LEFT;
        if (minDelta == deltaRight) return BodyCollisionSide::RIGHT;
        if (minDelta == deltaTop) return BodyCollisionSide::TOP;
        if (minDelta == deltaBottom) return BodyCollisionSide::BOTTOM;

        return BodyCollisionSide::NONE;
    }

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    CollisionArea::CollisionArea(glm::vec3 position, glm::vec3 size, CollisionType type)
        : areaType(type), areaPosition(position), areaSize(size)
    {
    }

    glm::vec3 CollisionArea::GetPosition() const
    {
        return areaPosition;
    }

    glm::vec3 CollisionArea::GetSize() const
    {
        return areaSize;
    }

    bool CollisionArea::BodiesOverlap(Body *body1, Body *body2) const
    {
        return body1->GetPosition().x < body2->GetPosition().x + body2->GetSize().x &&
               body1->GetPosition().x + body1->GetSize().x > body2->GetPosition().x &&
               body1->GetPosition().y < body2->GetPosition().y + body2->GetSize().y &&
               body1->GetPosition().y + body1->GetSize().y > body2->GetPosition().y &&
               body1->GetPosition().z < body2->GetPosition().z + body2->GetSize().z &&
               body1->GetPosition().z + body1->GetSize().z > body2->GetPosition().z;
    }

    BodyCollisionSide CollisionArea::GetCollisionSide(Body *body1, Body *body2) const
    {
        float deltaLeft = (body2->GetPosition().x + body2->GetSize().x) - body1->GetPosition().x;
        float deltaRight = (body1->GetPosition().x + body1->GetSize().x) - body2->GetPosition().x;
        float deltaTop = (body2->GetPosition().y + body2->GetSize().y) - body1->GetPosition().y;
        float deltaBottom = (body1->GetPosition().y + body1->GetSize().y) - body2->GetPosition().y;
        float deltaFront = (body2->GetPosition().z + body2->GetSize().z) - body1->GetPosition().z;
        float deltaBack = (body1->GetPosition().z + body1->GetSize().z) - body2->GetPosition().z;

        float minDelta = std::min({deltaLeft, deltaRight, deltaTop, deltaBottom, deltaFront, deltaBack});

        if (minDelta == deltaLeft) return BodyCollisionSide::LEFT;
        if (minDelta == deltaRight) return BodyCollisionSide::RIGHT;
        if (minDelta == deltaTop) return BodyCollisionSide::TOP;
        if (minDelta == deltaBottom) return BodyCollisionSide::BOTTOM;
        if (minDelta == deltaFront) return BodyCollisionSide::FRONT;
        if (minDelta == deltaBack) return BodyCollisionSide::BACK;

        return BodyCollisionSide::NONE;
    }

#else
    #error "[CollisionArea] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

    CollisionType CollisionArea::GetType() const
    {
        return areaType;
    }
}