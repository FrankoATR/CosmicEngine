#ifndef COSMIC_COLLISIONAREA2D_HPP
#define COSMIC_COLLISIONAREA2D_HPP

#include <glm/glm.hpp>

namespace CosmicEngine
{
    class Body;
    enum class BodyCollisionSide : int;

    enum class CollisionType
    {
        Grid,
        QuadTree
    };

    class CollisionArea2D
    {
    protected:
        glm::vec2 areaPosition;
        glm::vec2 areaSize;
        CollisionType areaType;

        CollisionArea2D(glm::vec2 position, glm::vec2 size, CollisionType type);

        bool RectToRectCollisionBody(Body *body1, Body *body2) const;
        BodyCollisionSide GetCollisionSide(Body *body1, Body *body2) const;

    public:
        virtual ~CollisionArea2D() = default;

        CollisionType GetType() const;

        virtual glm::vec2 GetPosition() const;
        virtual void SetPosition(glm::vec2 newPosition) = 0;
        virtual glm::vec2 GetSize() const;

        virtual void DrawDebug() = 0;
        virtual void AddObject(Body *body) = 0;
        virtual void RemoveObject(Body *body) = 0;
        virtual void FindCollisions() = 0;
        virtual void Clear() = 0;
    };
}

#endif // COSMIC_COLLISIONAREA2D_HPP