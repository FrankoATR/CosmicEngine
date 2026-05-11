#include "ObjectManager.hpp"
#include "../Models/GameObject.hpp"

namespace WandEngine
{
    ObjectManager::ObjectManager() : nextEntityId(0)
    {
        
    }

    ObjectManager::~ObjectManager()
    {
        Clear();
    }

    void ObjectManager::Update(float deltaTime)
    {
        std::vector<int> toRemove;

        for (auto actor : actors)
        {
            actor->GetAliveInGameManager() ? actor->Update(deltaTime) : toRemove.push_back(actor->GetObjectId());
        }

        for (int id : toRemove)
        {
            Remove(id);
        }
    }

    void ObjectManager::Draw()
    {
        for (auto actor : actors)
        {
            actor->Draw();
        }
    }

    void ObjectManager::Add(GameObject *actor)
    {
        actor->SetObjectId(nextEntityId++);
        actor->Init();
        actors.push_back(actor);

        SortByLayer();
    }

    void ObjectManager::Remove(int entityId)
    {
        auto it = std::find_if(actors.begin(), actors.end(), [entityId](GameObject *obj)
                               { return obj->GetObjectId() == entityId; });
        if (it != actors.end())
        {
            delete *it;
            actors.erase(it);
        }
    }

    void ObjectManager::SortByLayer()
    {
        std::sort(actors.begin(), actors.end(), [](GameObject *a, GameObject *b)
                  { return a->GetLayerId() < b->GetLayerId(); });
    }

    void ObjectManager::Clear()
    {
        for (const auto &actor : actors)
        {
            delete actor;
        }
        actors.clear();
        nextEntityId = 0;
    }

}
