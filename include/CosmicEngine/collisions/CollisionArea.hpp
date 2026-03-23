#ifndef COSMIC_COLLISIONAREA_HPP
#define COSMIC_COLLISIONAREA_HPP

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

        CollisionType GetType() const;

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        virtual glm::vec2 GetPosition() const;
        virtual void SetPosition(glm::vec2 newPosition) = 0;
        virtual glm::vec2 GetSize() const;

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        virtual glm::vec3 GetPosition() const;
        virtual void SetPosition(glm::vec3 newPosition) = 0;
        virtual glm::vec3 GetSize() const;

#else
        #error "[CollisionArea] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

        virtual void DrawDebug() = 0;
        virtual void AddObject(Body *body) = 0;
        virtual void RemoveObject(Body *body) = 0;
        virtual void FindCollisions() = 0;
        virtual void Clear() = 0;
    };
}

#endif // COSMIC_COLLISIONAREA_HPP