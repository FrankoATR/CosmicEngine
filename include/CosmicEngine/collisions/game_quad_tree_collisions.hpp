#ifndef COSMIC_GAMEQUADTREECOLLISIONS_HPP
#define COSMIC_GAMEQUADTREECOLLISIONS_HPP

/**
 * @file game_quad_tree_collisions.hpp
 * @brief Declares the quadtree or octree collision area used by the engine.
 */

#include "collision_area.hpp"

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
        /**
         * @brief Internal node of the quadtree (2D) or octree (3D).
         *
         * Each node owns either zero (leaf) or 4/8 children covering a uniform
         * subdivision of its bounding box.  Bodies are stored only in nodes that
         * fully contain them, falling back to the deepest fitting ancestor.
         */
        struct QuadNode
        {
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
            glm::vec2 position;                               ///< Top-left corner of the node AABB.
            glm::vec2 size;                                   ///< Width and height of the node AABB.
            std::array<std::unique_ptr<QuadNode>, 4> children;///< Owned child quadrants (NW, NE, SW, SE).

            /** @brief Builds a 2D quad node covering the given AABB at the given depth. */
            QuadNode(glm::vec2 position, glm::vec2 size, int depth);

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
            glm::vec3 position;                               ///< Min corner of the node AABB.
            glm::vec3 size;                                   ///< Extents of the node AABB.
            std::array<std::unique_ptr<QuadNode>, 8> children;///< Owned child octants.

            /** @brief Builds a 3D oct node covering the given AABB at the given depth. */
            QuadNode(glm::vec3 position, glm::vec3 size, int depth);

#else
            #error "[GameQuadTreeCollisions] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

            int depth;                                       ///< Depth of this node within the tree (root = 0).
                        /** @brief Bodies currently stored in the node. */
            std::vector<Body *> objects;
                        /**
                         * @brief Returns whether the node has no children.
                         * @return The whether the node has no children.
                         */
            bool IsLeaf() const;
        };

        std::unique_ptr<QuadNode> root; ///< Root node owning the whole partition tree.
        int maxDepth;                   ///< Maximum subdivision depth allowed.
        int maxObjectsPerNode;          ///< Threshold of bodies before a node is split.

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
        /**
         * @brief Builds a 2D quadtree covering the AABB defined by @p position and @p size.
         * @param position Top-left corner of the area in world space.
         * @param size Width and height of the area in world units.
         * @param maxDepth Maximum recursive subdivision depth.
         * @param maxObjectsPerNode Bodies per node before a split is triggered.
         */
        GameQuadTreeCollisions(glm::vec2 position, glm::vec2 size, int maxDepth = 5, int maxObjectsPerNode = 4);
        /** @brief Returns the world-space origin (top-left corner) of the partition area. */
        glm::vec2 GetPosition() const override;
        /** @brief Repositions the partition area and rebuilds the root node. */
        void SetPosition(glm::vec2 newPosition) override;

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
        /**
         * @brief Builds a 3D octree covering the AABB defined by @p position and @p size.
         * @param position Min corner of the volume in world space.
         * @param size Extents of the volume in world units.
         * @param maxDepth Maximum recursive subdivision depth.
         * @param maxObjectsPerNode Bodies per node before a split is triggered.
         */
        GameQuadTreeCollisions(glm::vec3 position, glm::vec3 size, int maxDepth = 5, int maxObjectsPerNode = 4);
        /** @brief Returns the world-space min corner of the partition volume. */
        glm::vec3 GetPosition() const override;
        /** @brief Repositions the partition volume and rebuilds the root node. */
        void SetPosition(glm::vec3 newPosition) override;

#else
        #error "[GameQuadTreeCollisions] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

        /** @brief Releases the entire partition tree and its tracked bodies. */
        ~GameQuadTreeCollisions();
        /** @brief Renders every node AABB and the bounding volume of every body (debug helper). */
        void DrawDebug() override;
        /** @brief Inserts a body into the deepest node that fully contains its AABB. */
        void AddObject(Body *body) override;
        /** @brief Removes a body from the partition tree. */
        void RemoveObject(Body *body) override;
        /** @brief Walks the tree and dispatches collision callbacks for overlapping bodies. */
        void FindCollisions() override;
        /** @brief Empties the tree without releasing the root node allocation. */
        void Clear() override;
    };
}

#endif // COSMIC_GAMEQUADTREECOLLISIONS_HPP
