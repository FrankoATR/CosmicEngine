#ifndef COSMIC_COLLISIONAREA_HPP
#define COSMIC_COLLISIONAREA_HPP

/**
 * @file CollisionArea.hpp
 * @brief Declares the abstract collision area used by the engine collision system.
 */

#include <glm/glm.hpp>

namespace CosmicEngine
{
    class Body;
    enum class BodyCollisionSide : int;

        /**
         * @brief Supported collision area acceleration structures.
         */
    enum class CollisionType
    {
        Grid,
        QuadTree
    };

        /**
         * @brief Base class for collision acceleration structures.
         *
         * CollisionArea is the abstract base that all spatial collision strategies
         * (grid, quadtree) extend.  You do not instantiate it directly; instead,
         * call BodyManager::CreateCollisionArea() which selects the correct subclass.
         *
         * @par Example — creating a collision area through the BodyManager
         * @code
         * BOD_MN.CreateCollisionArea(
         *     CosmicEngine::CollisionType::Grid,
         *     glm::vec2(-500, -300),   // area position
         *     glm::vec2(1000, 600),    // area size
         *     80,    // cell/subdivision size
         *     6,     // max depth
         *     4);    // max objects per node
         * @endcode
         */
    class CollisionArea
    {
    protected:
        CollisionType areaType;

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        glm::vec2 areaPosition;
        glm::vec2 areaSize;

        CollisionArea(glm::vec2 position, glm::vec2 size, CollisionType type);

        bool BodiesOverlap(Body *body1, Body *body2) const;
        BodyCollisionSide GetCollisionSide(Body *body1, Body *body2) const;

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        glm::vec3 areaPosition;
        glm::vec3 areaSize;

        CollisionArea(glm::vec3 position, glm::vec3 size, CollisionType type);

        bool BodiesOverlap(Body *body1, Body *body2) const;
        BodyCollisionSide GetCollisionSide(Body *body1, Body *body2) const;

#else
        #error "[CollisionArea] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

    public:
        virtual ~CollisionArea() = default;

                /** @brief Returns the collision area type. */
        CollisionType GetType() const;

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        /** @brief Returns the origin of the collision area. */
        virtual glm::vec2 GetPosition() const;
        /** @brief Sets the origin of the collision area. */
        virtual void SetPosition(glm::vec2 newPosition) = 0;
        /** @brief Returns the size of the collision area. */
        virtual glm::vec2 GetSize() const;

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        /** @brief Returns the origin of the collision area. */
        virtual glm::vec3 GetPosition() const;
        /** @brief Sets the origin of the collision area. */
        virtual void SetPosition(glm::vec3 newPosition) = 0;
        /** @brief Returns the size of the collision area. */
        virtual glm::vec3 GetSize() const;

#else
        #error "[CollisionArea] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

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

#endif // COSMIC_COLLISIONAREA_HPP