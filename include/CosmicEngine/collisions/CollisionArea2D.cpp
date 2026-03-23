#include "CollisionArea2D.hpp"

#include "../models/body/body.hpp"

#include <algorithm>

namespace CosmicEngine
{
    CollisionArea2D::CollisionArea2D(glm::vec2 position, glm::vec2 size, CollisionType type)
        : areaPosition(position), areaSize(size), areaType(type)
    {
    }

    CollisionType CollisionArea2D::GetType() const
    {
        return areaType;
    }

    glm::vec2 CollisionArea2D::GetPosition() const
    {
        return areaPosition;
    }

    glm::vec2 CollisionArea2D::GetSize() const
    {
        return areaSize;
    }

    bool CollisionArea2D::RectToRectCollisionBody(Body *body1, Body *body2) const
    {
        return (body1->GetPosition().x < body2->GetPosition().x + body2->GetSize().x &&
                body1->GetPosition().x + body1->GetSize().x > body2->GetPosition().x &&
                body1->GetPosition().y < body2->GetPosition().y + body2->GetSize().y &&
                body1->GetPosition().y + body1->GetSize().y > body2->GetPosition().y);
    }

    BodyCollisionSide CollisionArea2D::GetCollisionSide(Body *body1, Body *body2) const
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
}