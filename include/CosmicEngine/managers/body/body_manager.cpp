/**
 * @file body_manager.cpp
 * @brief Implements the runtime collision body manager used by the engine.
 */

#include "body_manager.hpp"

#include "../../collisions/CollisionArea.hpp"
#include "../../collisions/GameGridCollisions.hpp"
#include "../../collisions/GameQuadTreeCollisions.hpp"
#include "../../models/body/body.hpp"
#include "../../models/object/object.hpp"
#include "../../utils/log.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace CosmicEngine
{
    BodyManager &BodyManager::GetInstance()
    {
        static BodyManager instance;
        return instance;
    }

    BodyManager::BodyManager()
        : nextEntityId(0), collisionArea(nullptr)
    {
        RUNTIME_INFO("Body manager created");
    }

    BodyManager::~BodyManager()
    {
        if (collisionArea)
        {
            delete collisionArea;
            collisionArea = nullptr;
        }

        RUNTIME_INFO("Body manager destroyed");
    }

    void BodyManager::init()
    {
        collisionArea = nullptr;
        nextEntityId = 0;
        toDelete.clear();
        RUNTIME_INFO("Body manager initialized");
    }

    void BodyManager::draw()
    {
        if (collisionArea)
        {
            collisionArea->DrawDebug();
        }

        for (Body *body : bodys)
        {
            if (body && body->GetParent() && body->GetParent()->GetVisible())
            {
                body->draw();
            }
        }
    }

    void BodyManager::update()
    {
		// Bodies are first synchronized from their parents, then inserted into the active collision area for broad-phase checks.
        for (int id : toDelete)
        {
            Remove(id);
        }
        toDelete.clear();

        if (collisionArea)
        {
            collisionArea->Clear();
        }

        for (Body *body : bodys)
        {
            if (!body || !body->GetParent())
            {
                continue;
            }

            if (body->GetParent()->IsAlive() && body->IsAlive())
            {
                body->update();

                if (collisionArea)
                {
                    collisionArea->AddObject(body);
                }
            }
            else
            {
                toDelete.push_back(body->GetID());
            }
        }

        if (collisionArea)
        {
            collisionArea->FindCollisions();
        }
    }

    void BodyManager::Add(Body *body)
    {
        if (body && body->GetParent())
        {
            body->SetID(nextEntityId++);
            bodys.push_back(body);
            return;
        }

        delete body;
        std::cerr << "Body needs to have a parent" << std::endl;
    }

    std::vector<Body *> BodyManager::FindAllByParent(Object *parent)
    {
        std::vector<Body *> bodiesFound;
        for (Body *body : bodys)
        {
            if (body && body->GetParent() == parent)
            {
                bodiesFound.push_back(body);
            }
        }

        return bodiesFound;
    }

    void BodyManager::Remove(int entityId)
    {
        auto it = std::find_if(bodys.begin(), bodys.end(), [entityId](Body *body) {
            return body && body->GetID() == entityId;
        });

        if (it != bodys.end())
        {
            Body *body = *it;
            bodys.erase(it);
            delete body;
        }
    }

    void BodyManager::SetCollisionArea(CollisionArea *newCollisionArea)
    {
        if (collisionArea)
        {
            collisionArea->Clear();
            delete collisionArea;
        }

        collisionArea = newCollisionArea;
    }

    void BodyManager::SetNewGridArea(GameGridCollisions *newGridArea)
    {
        SetCollisionArea(newGridArea);
    }

    void BodyManager::SetNewQuadTreeArea(GameQuadTreeCollisions *newQuadTreeArea)
    {
        SetCollisionArea(newQuadTreeArea);
    }

    CollisionType BodyManager::GetCollisionAreaType() const
    {
        if (!collisionArea)
        {
            return CollisionType::Grid;
        }

        return collisionArea->GetType();
    }

    bool BodyManager::HasCollisionArea() const
    {
        return collisionArea != nullptr;
    }

#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    void BodyManager::CreateCollisionArea(CollisionType type, glm::vec2 position, glm::vec2 size, int subdivisionSize, int maxDepth, int maxObjectsPerNode)
    {
        if (size.x <= 0.0f || size.y <= 0.0f)
        {
            RUNTIME_WARNING("Collision area size must be greater than zero");
            return;
        }

        switch (type)
        {
            case CollisionType::Grid:
            {
                int safeCellSize = std::max(1, subdivisionSize);
                int columns = std::max(1, static_cast<int>(std::ceil(size.x / static_cast<float>(safeCellSize))));
                int arrows = std::max(1, static_cast<int>(std::ceil(size.y / static_cast<float>(safeCellSize))));
                SetNewGridArea(new GameGridCollisions(position, arrows, columns, safeCellSize));
                break;
            }

            case CollisionType::QuadTree:
                SetNewQuadTreeArea(new GameQuadTreeCollisions(position, size, maxDepth, maxObjectsPerNode));
                break;
        }
    }

    void BodyManager::SetGridPosition(glm::vec2 newPosition)
    {
        if (collisionArea)
        {
            collisionArea->SetPosition(newPosition);
        }
    }

    glm::vec2 BodyManager::GetGridPosition()
    {
        if (collisionArea)
        {
            return collisionArea->GetPosition();
        }

        return glm::vec2(0.0f);
    }

#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
    void BodyManager::CreateCollisionArea(CollisionType type, glm::vec3 position, glm::vec3 size, int subdivisionSize, int maxDepth, int maxObjectsPerNode)
    {
        if (size.x <= 0.0f || size.y <= 0.0f || size.z <= 0.0f)
        {
            RUNTIME_WARNING("Collision area size must be greater than zero");
            return;
        }

        switch (type)
        {
            case CollisionType::Grid:
            {
                int safeCellSize = std::max(1, subdivisionSize);
                int columns = std::max(1, static_cast<int>(std::ceil(size.x / static_cast<float>(safeCellSize))));
                int rows = std::max(1, static_cast<int>(std::ceil(size.y / static_cast<float>(safeCellSize))));
                int layers = std::max(1, static_cast<int>(std::ceil(size.z / static_cast<float>(safeCellSize))));
                SetNewGridArea(new GameGridCollisions(position, rows, columns, layers, safeCellSize));
                break;
            }

            case CollisionType::QuadTree:
                SetNewQuadTreeArea(new GameQuadTreeCollisions(position, size, maxDepth, maxObjectsPerNode));
                break;
        }
    }

    void BodyManager::SetGridPosition(glm::vec3 newPosition)
    {
        if (collisionArea)
        {
            collisionArea->SetPosition(newPosition);
        }
    }

    glm::vec3 BodyManager::GetGridPosition()
    {
        if (collisionArea)
        {
            return collisionArea->GetPosition();
        }

        return glm::vec3(0.0f);
    }

#else
    #error "[BodyManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

    void BodyManager::Clear()
    {
        if (collisionArea)
        {
            collisionArea->Clear();
            delete collisionArea;
            collisionArea = nullptr;
        }

        while (!bodys.empty())
        {
            Body *body = bodys.back();
            if (body)
            {
                delete body;
            }
            bodys.pop_back();
        }

        bodys.clear();
        toDelete.clear();
        nextEntityId = 0;

        RUNTIME_INFO("Body manager cleared");
    }
}
