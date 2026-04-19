#ifndef COSMIC_COLLISIONAREA2D_HPP
#define COSMIC_COLLISIONAREA2D_HPP

/**
 * @file CollisionArea2D.hpp
 * @brief Declares the legacy 2D-only collision area base type.
 */

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

    /**
     * @brief Legacy 2D collision area abstraction retained for compatibility.
     *
     * CollisionArea2D serves as a minimal 2D-only base class from earlier
     * engine versions.  Modern code should prefer **CollisionArea** which
     * supports both 2D and 3D modes.  Bodies are added/removed automatically
     * by BodyManager when a collision area exists.
     *
     * @see CollisionArea, GameGridCollisions, GameQuadTreeCollisions
     */
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

        /** @brief Returns the collision area type. */
        CollisionType GetType() const;

        /** @brief Returns the origin of the collision area. */
        virtual glm::vec2 GetPosition() const;
        /** @brief Sets the origin of the collision area. */
        virtual void SetPosition(glm::vec2 newPosition) = 0;
        /** @brief Returns the size of the collision area. */
        virtual glm::vec2 GetSize() const;

        /** @brief Draws debug visualization for the collision area. */
        virtual void DrawDebug() = 0;
        /** @brief Adds a body to the collision area. */
        virtual void AddObject(Body *body) = 0;
        /** @brief Removes a body from the collision area. */
        virtual void RemoveObject(Body *body) = 0;
        /** @brief Finds and dispatches collisions for the currently stored bodies. */
        virtual void FindCollisions() = 0;
        /** @brief Clears all bodies and internal state from the collision area. */
        virtual void Clear() = 0;
    };
}

#endif // COSMIC_COLLISIONAREA2D_HPP