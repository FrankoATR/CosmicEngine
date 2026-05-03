/**
 * @file game_quad_tree_collisions.cpp
 * @brief Implements the quadtree or octree collision accelerator used by the engine.
 */

#include "game_quad_tree_collisions.hpp"

#include "../managers/resource/resource_manager.hpp"
#include "../models/body/body.hpp"
#include "../models/object/object.hpp"

#include <algorithm>

namespace CosmicEngine
{
#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    GameQuadTreeCollisions::QuadNode::QuadNode(glm::vec2 position, glm::vec2 size, int depth)
        : position(position), size(size), depth(depth)
    {
    }

    bool GameQuadTreeCollisions::QuadNode::IsLeaf() const
    {
        return !children[0];
    }

    GameQuadTreeCollisions::GameQuadTreeCollisions(glm::vec2 position, glm::vec2 size, int maxDepth, int maxObjectsPerNode)
        : CollisionArea(position, size, CollisionType::QuadTree),
          root(std::make_unique<QuadNode>(position, size, 0)),
          maxDepth(std::max(1, maxDepth)),
          maxObjectsPerNode(std::max(1, maxObjectsPerNode))
    {
    }

    glm::vec2 GameQuadTreeCollisions::GetPosition() const
    {
        return areaPosition;
    }

    void GameQuadTreeCollisions::SetPosition(glm::vec2 newPosition)
    {
        areaPosition = newPosition;
        root = std::make_unique<QuadNode>(areaPosition, areaSize, 0);
    }

    bool GameQuadTreeCollisions::IsBodyInsideArea(Body *body) const
    {
        glm::vec2 bodyPosition = body->GetPosition();
        glm::vec2 bodySize = body->GetSize();

        return bodyPosition.x >= areaPosition.x &&
               bodyPosition.y >= areaPosition.y &&
               bodyPosition.x + bodySize.x <= areaPosition.x + areaSize.x &&
               bodyPosition.y + bodySize.y <= areaPosition.y + areaSize.y;
    }

    bool GameQuadTreeCollisions::IntersectsNode(const QuadNode *node, Body *body) const
    {
        glm::vec2 bodyPosition = body->GetPosition();
        glm::vec2 bodySize = body->GetSize();

        return bodyPosition.x < node->position.x + node->size.x &&
               bodyPosition.x + bodySize.x > node->position.x &&
               bodyPosition.y < node->position.y + node->size.y &&
               bodyPosition.y + bodySize.y > node->position.y;
    }

    bool GameQuadTreeCollisions::FitsCompletelyInNode(const QuadNode *node, Body *body) const
    {
        glm::vec2 bodyPosition = body->GetPosition();
        glm::vec2 bodySize = body->GetSize();

        return bodyPosition.x >= node->position.x &&
               bodyPosition.y >= node->position.y &&
               bodyPosition.x + bodySize.x <= node->position.x + node->size.x &&
               bodyPosition.y + bodySize.y <= node->position.y + node->size.y;
    }

    int GameQuadTreeCollisions::GetChildIndex(const QuadNode *node, Body *body) const
    {
        if (node->IsLeaf())
        {
            return -1;
        }

        for (int index = 0; index < 4; ++index)
        {
            if (FitsCompletelyInNode(node->children[index].get(), body))
            {
                return index;
            }
        }

        return -1;
    }

    void GameQuadTreeCollisions::Subdivide(QuadNode *node)
    {
        if (!node || !node->IsLeaf())
        {
            return;
        }

        glm::vec2 halfSize = node->size * 0.5f;
        if (halfSize.x <= 0.0f || halfSize.y <= 0.0f)
        {
            return;
        }

        node->children[0] = std::make_unique<QuadNode>(node->position, halfSize, node->depth + 1);
        node->children[1] = std::make_unique<QuadNode>(glm::vec2(node->position.x + halfSize.x, node->position.y), halfSize, node->depth + 1);
        node->children[2] = std::make_unique<QuadNode>(glm::vec2(node->position.x, node->position.y + halfSize.y), halfSize, node->depth + 1);
        node->children[3] = std::make_unique<QuadNode>(node->position + halfSize, halfSize, node->depth + 1);
    }

    void GameQuadTreeCollisions::InsertBody(QuadNode *node, Body *body)
    {
        if (!node || !IntersectsNode(node, body))
        {
            return;
        }

        if (!node->IsLeaf())
        {
            int childIndex = GetChildIndex(node, body);
            if (childIndex >= 0)
            {
                InsertBody(node->children[childIndex].get(), body);
                return;
            }
        }

        node->objects.push_back(body);

        if (node->objects.size() > static_cast<size_t>(maxObjectsPerNode) && node->depth < maxDepth)
        {
            if (node->IsLeaf())
            {
                Subdivide(node);
            }

            RedistributeNodeObjects(node);
        }
    }

    void GameQuadTreeCollisions::RedistributeNodeObjects(QuadNode *node)
    {
        if (!node || node->IsLeaf())
        {
            return;
        }

        auto it = node->objects.begin();
        while (it != node->objects.end())
        {
            int childIndex = GetChildIndex(node, *it);
            if (childIndex >= 0)
            {
                Body *body = *it;
                it = node->objects.erase(it);
                InsertBody(node->children[childIndex].get(), body);
            }
            else
            {
                ++it;
            }
        }
    }

    void GameQuadTreeCollisions::CollectCollisions(QuadNode *node, const std::vector<Body *> &ancestorObjects)
    {
        if (!node)
        {
            return;
        }

        // Compare local bodies first, then against ancestors so parent-child overlaps are not missed.
        CompareBodies(node->objects, node->objects, true);
        CompareBodies(node->objects, ancestorObjects, false);

        std::vector<Body *> nextAncestors = ancestorObjects;
        nextAncestors.insert(nextAncestors.end(), node->objects.begin(), node->objects.end());

        if (!node->IsLeaf())
        {
            for (const auto &child : node->children)
            {
                CollectCollisions(child.get(), nextAncestors);
            }
        }
    }

    void GameQuadTreeCollisions::CompareBodies(const std::vector<Body *> &leftBodies, const std::vector<Body *> &rightBodies, bool sameContainer)
    {
        for (size_t leftIndex = 0; leftIndex < leftBodies.size(); ++leftIndex)
        {
            size_t startIndex = sameContainer ? leftIndex + 1 : 0;
            for (size_t rightIndex = startIndex; rightIndex < rightBodies.size(); ++rightIndex)
            {
                Body *leftBody = leftBodies[leftIndex];
                Body *rightBody = rightBodies[rightIndex];

                if (!leftBody || !rightBody || leftBody == rightBody)
                {
                    continue;
                }

                if (!leftBody->GetParent()->IsAlive() || !rightBody->GetParent()->IsAlive())
                {
                    continue;
                }

                if (BodiesOverlap(leftBody, rightBody))
                {
                    leftBody->OnCollision(rightBody, GetCollisionSide(leftBody, rightBody));
                }
            }
        }
    }

    void GameQuadTreeCollisions::DrawNode(const QuadNode *node)
    {
        if (!node)
        {
            return;
        }

        if (!node->objects.empty())
        {
            glm::vec3 lineColor(
                std::min(0.20f + 0.10f * static_cast<float>(node->depth), 0.70f),
                std::min(0.85f, 0.45f + 0.12f * static_cast<float>(node->objects.size())),
                std::min(0.15f + 0.05f * static_cast<float>(node->depth), 0.45f));

            ResourceManager::GetInstance().RenderRectangle(
                node->position,
                node->position + node->size,
                glm::vec2(0.0f),
                glm::vec2(0.0f),
                lineColor,
                1.0f,
                2.5f,
                false,
                ViewType::Ortho);
        }

        if (!node->IsLeaf())
        {
            for (const auto &child : node->children)
            {
                DrawNode(child.get());
            }
        }
    }

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    GameQuadTreeCollisions::QuadNode::QuadNode(glm::vec3 position, glm::vec3 size, int depth)
        : position(position), size(size), depth(depth)
    {
    }

    bool GameQuadTreeCollisions::QuadNode::IsLeaf() const
    {
        return !children[0];
    }

    GameQuadTreeCollisions::GameQuadTreeCollisions(glm::vec3 position, glm::vec3 size, int maxDepth, int maxObjectsPerNode)
        : CollisionArea(position, size, CollisionType::QuadTree),
          root(std::make_unique<QuadNode>(position, size, 0)),
          maxDepth(std::max(1, maxDepth)),
          maxObjectsPerNode(std::max(1, maxObjectsPerNode))
    {
    }

    glm::vec3 GameQuadTreeCollisions::GetPosition() const
    {
        return areaPosition;
    }

    void GameQuadTreeCollisions::SetPosition(glm::vec3 newPosition)
    {
        areaPosition = newPosition;
        root = std::make_unique<QuadNode>(areaPosition, areaSize, 0);
    }

    bool GameQuadTreeCollisions::IsBodyInsideArea(Body *body) const
    {
        glm::vec3 bodyPosition = body->GetPosition();
        glm::vec3 bodySize = body->GetSize();

        return bodyPosition.x >= areaPosition.x &&
               bodyPosition.y >= areaPosition.y &&
               bodyPosition.z >= areaPosition.z &&
               bodyPosition.x + bodySize.x <= areaPosition.x + areaSize.x &&
               bodyPosition.y + bodySize.y <= areaPosition.y + areaSize.y &&
               bodyPosition.z + bodySize.z <= areaPosition.z + areaSize.z;
    }

    bool GameQuadTreeCollisions::IntersectsNode(const QuadNode *node, Body *body) const
    {
        glm::vec3 bodyPosition = body->GetPosition();
        glm::vec3 bodySize = body->GetSize();

        return bodyPosition.x < node->position.x + node->size.x &&
               bodyPosition.x + bodySize.x > node->position.x &&
               bodyPosition.y < node->position.y + node->size.y &&
               bodyPosition.y + bodySize.y > node->position.y &&
               bodyPosition.z < node->position.z + node->size.z &&
               bodyPosition.z + bodySize.z > node->position.z;
    }

    bool GameQuadTreeCollisions::FitsCompletelyInNode(const QuadNode *node, Body *body) const
    {
        glm::vec3 bodyPosition = body->GetPosition();
        glm::vec3 bodySize = body->GetSize();

        return bodyPosition.x >= node->position.x &&
               bodyPosition.y >= node->position.y &&
               bodyPosition.z >= node->position.z &&
               bodyPosition.x + bodySize.x <= node->position.x + node->size.x &&
               bodyPosition.y + bodySize.y <= node->position.y + node->size.y &&
               bodyPosition.z + bodySize.z <= node->position.z + node->size.z;
    }

    int GameQuadTreeCollisions::GetChildIndex(const QuadNode *node, Body *body) const
    {
        if (node->IsLeaf())
        {
            return -1;
        }

        for (int index = 0; index < 8; ++index)
        {
            if (FitsCompletelyInNode(node->children[index].get(), body))
            {
                return index;
            }
        }

        return -1;
    }

    void GameQuadTreeCollisions::Subdivide(QuadNode *node)
    {
        if (!node || !node->IsLeaf())
        {
            return;
        }

        glm::vec3 halfSize = node->size * 0.5f;
        if (halfSize.x <= 0.0f || halfSize.y <= 0.0f || halfSize.z <= 0.0f)
        {
            return;
        }

        int childIndex = 0;
        for (int zOffset = 0; zOffset < 2; ++zOffset)
        {
            for (int yOffset = 0; yOffset < 2; ++yOffset)
            {
                for (int xOffset = 0; xOffset < 2; ++xOffset)
                {
                    glm::vec3 childPosition = node->position + glm::vec3(
                        xOffset * halfSize.x,
                        yOffset * halfSize.y,
                        zOffset * halfSize.z);

                    node->children[childIndex++] = std::make_unique<QuadNode>(childPosition, halfSize, node->depth + 1);
                }
            }
        }
    }

    void GameQuadTreeCollisions::InsertBody(QuadNode *node, Body *body)
    {
        if (!node || !IntersectsNode(node, body))
        {
            return;
        }

        if (!node->IsLeaf())
        {
            int childIndex = GetChildIndex(node, body);
            if (childIndex >= 0)
            {
                InsertBody(node->children[childIndex].get(), body);
                return;
            }
        }

        node->objects.push_back(body);

        if (node->objects.size() > static_cast<size_t>(maxObjectsPerNode) && node->depth < maxDepth)
        {
            if (node->IsLeaf())
            {
                Subdivide(node);
            }

            RedistributeNodeObjects(node);
        }
    }

    void GameQuadTreeCollisions::RedistributeNodeObjects(QuadNode *node)
    {
        if (!node || node->IsLeaf())
        {
            return;
        }

        auto it = node->objects.begin();
        while (it != node->objects.end())
        {
            int childIndex = GetChildIndex(node, *it);
            if (childIndex >= 0)
            {
                Body *body = *it;
                it = node->objects.erase(it);
                InsertBody(node->children[childIndex].get(), body);
            }
            else
            {
                ++it;
            }
        }
    }

    void GameQuadTreeCollisions::CollectCollisions(QuadNode *node, const std::vector<Body *> &ancestorObjects)
    {
        if (!node)
        {
            return;
        }

        CompareBodies(node->objects, node->objects, true);
        CompareBodies(node->objects, ancestorObjects, false);

        std::vector<Body *> nextAncestors = ancestorObjects;
        nextAncestors.insert(nextAncestors.end(), node->objects.begin(), node->objects.end());

        if (!node->IsLeaf())
        {
            for (const auto &child : node->children)
            {
                CollectCollisions(child.get(), nextAncestors);
            }
        }
    }

    void GameQuadTreeCollisions::CompareBodies(const std::vector<Body *> &leftBodies, const std::vector<Body *> &rightBodies, bool sameContainer)
    {
        for (size_t leftIndex = 0; leftIndex < leftBodies.size(); ++leftIndex)
        {
            size_t startIndex = sameContainer ? leftIndex + 1 : 0;
            for (size_t rightIndex = startIndex; rightIndex < rightBodies.size(); ++rightIndex)
            {
                Body *leftBody = leftBodies[leftIndex];
                Body *rightBody = rightBodies[rightIndex];

                if (!leftBody || !rightBody || leftBody == rightBody)
                {
                    continue;
                }

                if (!leftBody->GetParent()->IsAlive() || !rightBody->GetParent()->IsAlive())
                {
                    continue;
                }

                if (BodiesOverlap(leftBody, rightBody))
                {
                    leftBody->OnCollision(rightBody, GetCollisionSide(leftBody, rightBody));
                }
            }
        }
    }

    void GameQuadTreeCollisions::DrawNode(const QuadNode *node)
    {
        if (!node)
        {
            return;
        }

        if (!node->objects.empty())
        {
            glm::vec3 lineColor(
                std::min(0.25f + 0.08f * static_cast<float>(node->depth), 0.70f),
                std::min(0.85f, 0.40f + 0.10f * static_cast<float>(node->objects.size())),
                std::min(0.30f + 0.07f * static_cast<float>(node->depth), 0.80f));
            const glm::vec3 nodeCenter = node->position + (node->size * 0.5f);

            ResourceManager::GetInstance().RenderParallelepipedLines(
                nodeCenter,
                node->size,
                glm::vec3(0.0f),
                glm::vec3(0.0f),
                lineColor,
                1.0f,
                2.0f,
                false,
                ViewType::Projection);
        }

        if (!node->IsLeaf())
        {
            for (const auto &child : node->children)
            {
                DrawNode(child.get());
            }
        }
    }

#else
    #error "[GameQuadTreeCollisions] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

    GameQuadTreeCollisions::~GameQuadTreeCollisions()
    {
        Clear();
    }

    void GameQuadTreeCollisions::DrawDebug()
    {
        DrawNode(root.get());
    }

    void GameQuadTreeCollisions::AddObject(Body *body)
    {
        if (!body || !body->GetParent())
        {
            return;
        }

        if (!IsBodyInsideArea(body))
        {
            body->GetParent()->SetInsideGridArea(false);
            return;
        }

        body->GetParent()->SetInsideGridArea(true);
        InsertBody(root.get(), body);
    }

    void GameQuadTreeCollisions::RemoveObject(Body *body)
    {
        (void)body;
    }

    void GameQuadTreeCollisions::FindCollisions()
    {
        std::vector<Body *> ancestorObjects;
        CollectCollisions(root.get(), ancestorObjects);
    }

    void GameQuadTreeCollisions::Clear()
    {
        root = std::make_unique<QuadNode>(areaPosition, areaSize, 0);
    }
}