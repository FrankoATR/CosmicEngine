#ifndef COSMIC_BODYMANAGER_HPP
#define COSMIC_BODYMANAGER_HPP

/**
 * @file body_manager.hpp
 * @brief Declares the collision body manager used by the engine.
 */

#include <glm/glm.hpp>
#include <vector>

namespace CosmicEngine
{

    class Body;
    class Object;
    enum class CollisionType : int;
    class CollisionArea;
    class GameGridCollisions;
    class GameQuadTreeCollisions;

    /**
     * @brief Owns runtime collision bodies and coordinates the active collision area.
     *
     * BodyManager stores every collision Body, assigns them unique IDs, and drives
     * the selected spatial acceleration structure (grid or quadtree) each frame.
     * You must call CreateCollisionArea() before bodies can detect collisions.
     *
     * @par Example — setting up a collision area and adding bodies
     * @code
     * // Create a 2D grid collision area (position, size, cellSize, maxDepth, maxObjects)
     * BOD_MN.CreateCollisionArea(
     *     CosmicEngine::CollisionType::Grid,
     *     glm::vec2(-500, -300), glm::vec2(1000, 600), 80, 6, 4);
     *
     * // Attach a body to an object:
     * CosmicEngine::Body* body = new CosmicEngine::Body(
     *     this, glm::vec2(0), GetSize(),
     *     CALLBACK_COLLISION_EVENT(OnCollision));
     * BOD_MN.Add(body);
     *
     * // Clean up:
     * BOD_MN.Remove(body->GetID());
     *
     * // Check if a collision area exists before spawning:
     * if (!BOD_MN.HasCollisionArea())
     *     BOD_MN.CreateCollisionArea(CosmicEngine::CollisionType::Grid, ...);
     * @endcode
     */
    class BodyManager
    {
    private:
        BodyManager();
        ~BodyManager();
        BodyManager(const BodyManager &) = delete;
        BodyManager &operator=(const BodyManager &) = delete;

        std::vector<Body *> bodys;
        std::vector<int> toDelete;
        int nextEntityId;
        CollisionArea *collisionArea;

    public:
        /** @brief Returns the singleton instance of the body manager. */
        static BodyManager &GetInstance();

        /** @brief Initializes the body manager state. */
        void init();
        /** @brief Draws all collision bodies for debug visualization. */
        void draw();
        /** @brief Updates all bodies and the active collision area. */
        void update();
        /** @brief Adds a collision body to the runtime and assigns it an identifier. */
        void Add(Body *body);
        /** @brief Removes and deletes a collision body by identifier. */
        void Remove(int entityId);
        /** @brief Deletes and removes every managed collision body. */
        void Clear();

        /** @brief Finds every body attached to the provided parent object. */
        std::vector<Body *> FindAllByParent(Object* parent);

        /** @brief Sets the active collision area implementation. */
        void SetCollisionArea(CollisionArea *newCollisionArea);
        /** @brief Sets a grid collision area as the active collision area. */
        void SetNewGridArea(GameGridCollisions *newGridArea);
        /** @brief Sets a quadtree collision area as the active collision area. */
        void SetNewQuadTreeArea(GameQuadTreeCollisions *newQuadTreeArea);
        /** @brief Returns the current collision area type. */
        CollisionType GetCollisionAreaType() const;
        /** @brief Returns whether a collision area is currently configured. */
        bool HasCollisionArea() const;

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            /** @brief Creates a 2D collision area with the provided configuration. */
            void CreateCollisionArea(CollisionType type, glm::vec2 position, glm::vec2 size, int subdivisionSize = 64, int maxDepth = 5, int maxObjectsPerNode = 4);
            /** @brief Sets the origin of the active 2D collision grid or area. */
            void SetGridPosition(glm::vec2 newPosition);
            /** @brief Returns the origin of the active 2D collision grid or area. */
            glm::vec2 GetGridPosition();

        #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            /** @brief Creates a 3D collision area with the provided configuration. */
            void CreateCollisionArea(CollisionType type, glm::vec3 position, glm::vec3 size, int subdivisionSize = 64, int maxDepth = 5, int maxObjectsPerNode = 4);
            /** @brief Sets the origin of the active 3D collision grid or area. */
            void SetGridPosition(glm::vec3 newPosition);
            /** @brief Returns the origin of the active 3D collision grid or area. */
            glm::vec3 GetGridPosition();

        #else
            #error "[BodyManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif
    };

}

#endif // COSMIC_BODYMANAGER_HPP