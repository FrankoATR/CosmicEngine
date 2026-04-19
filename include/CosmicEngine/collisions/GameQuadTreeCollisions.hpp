#ifndef COSMIC_GAMEQUADTREECOLLISIONS_HPP
#define COSMIC_GAMEQUADTREECOLLISIONS_HPP

/**
 * @file GameQuadTreeCollisions.hpp
 * @brief Declares the quadtree or octree collision area used by the engine.
 */

#include "CollisionArea.hpp"

#include <array>
#include <memory>
#include <vector>

namespace CosmicEngine
{
    class Body;

        /**
         * @brief Hierarchical collision accelerator used for spatial partitioning.
         *
         * In 2D mode this is a classic quadtree; in 3D mode it behaves as an octree.
         * Bodies are inserted into nodes that best fit their bounding volume, and
         * collision pairs are detected by walking the tree.  Created automatically by
         * BodyManager::CreateCollisionArea(CollisionType::QuadTree, ...).
         *
         * @par Example (automatic creation via BodyManager)
         * @code
         * BOD_MN.CreateCollisionArea(CosmicEngine::CollisionType::QuadTree,
         *     glm::vec2(-500, -300), glm::vec2(1000, 600), 80, 6, 4);
         * @endcode
         */
    class GameQuadTreeCollisions : public CollisionArea
    {
    private:
        struct QuadNode
        {
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            glm::vec2 position;
            glm::vec2 size;
            std::array<std::unique_ptr<QuadNode>, 4> children;

            QuadNode(glm::vec2 position, glm::vec2 size, int depth);

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            glm::vec3 position;
            glm::vec3 size;
            std::array<std::unique_ptr<QuadNode>, 8> children;

            QuadNode(glm::vec3 position, glm::vec3 size, int depth);

#else
            #error "[GameQuadTreeCollisions] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

            int depth;
                        /** @brief Bodies currently stored in the node. */
            std::vector<Body *> objects;
                        /** @brief Returns whether the node has no children. */
            bool IsLeaf() const;
        };

        std::unique_ptr<QuadNode> root;
        int maxDepth;
        int maxObjectsPerNode;

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        bool IsBodyInsideArea(Body *body) const;
        bool IntersectsNode(const QuadNode *node, Body *body) const;
        bool FitsCompletelyInNode(const QuadNode *node, Body *body) const;
        int GetChildIndex(const QuadNode *node, Body *body) const;
        void Subdivide(QuadNode *node);
        void InsertBody(QuadNode *node, Body *body);
        void RedistributeNodeObjects(QuadNode *node);
        void CollectCollisions(QuadNode *node, const std::vector<Body *> &ancestorObjects);
        void CompareBodies(const std::vector<Body *> &leftBodies, const std::vector<Body *> &rightBodies, bool sameContainer);
        void DrawNode(const QuadNode *node);

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        bool IsBodyInsideArea(Body *body) const;
        bool IntersectsNode(const QuadNode *node, Body *body) const;
        bool FitsCompletelyInNode(const QuadNode *node, Body *body) const;
        int GetChildIndex(const QuadNode *node, Body *body) const;
        void Subdivide(QuadNode *node);
        void InsertBody(QuadNode *node, Body *body);
        void RedistributeNodeObjects(QuadNode *node);
        void CollectCollisions(QuadNode *node, const std::vector<Body *> &ancestorObjects);
        void CompareBodies(const std::vector<Body *> &leftBodies, const std::vector<Body *> &rightBodies, bool sameContainer);
        void DrawNode(const QuadNode *node);

#else
        #error "[GameQuadTreeCollisions] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

    public:
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
        GameQuadTreeCollisions(glm::vec2 position, glm::vec2 size, int maxDepth = 5, int maxObjectsPerNode = 4);
        glm::vec2 GetPosition() const override;
        void SetPosition(glm::vec2 newPosition) override;

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        GameQuadTreeCollisions(glm::vec3 position, glm::vec3 size, int maxDepth = 5, int maxObjectsPerNode = 4);
        glm::vec3 GetPosition() const override;
        void SetPosition(glm::vec3 newPosition) override;

#else
        #error "[GameQuadTreeCollisions] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

        ~GameQuadTreeCollisions();
        void DrawDebug() override;
        void AddObject(Body *body) override;
        void RemoveObject(Body *body) override;
        void FindCollisions() override;
        void Clear() override;
    };
}

#endif // COSMIC_GAMEQUADTREECOLLISIONS_HPP