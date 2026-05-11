#include "ObjectManager.hpp"
#include "../Models/GameObject.hpp"

namespace WandEngine
{
    ObjectManager::ObjectManager() : nextEntityId(0)
    {
    }

    void ObjectManager::Update(float deltaTime)
    {
        std::vector<int> toRemove;

        for (auto actor : actors)
        {
            if (actor->GetAliveInGameManager())
            {
                if (actor->GetInsideGridArea())
                {
                    actor->Update(deltaTime);
                }
            }
            else
            {
                toRemove.push_back(actor->GetObjectId());
            }
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
            if (actor->GetVisible())
            {
                actor->Draw();
            }
        }
    }

    void ObjectManager::Add(GameObject *actor)
    {
        actor->SetObjectId(nextEntityId++);
        actor->Init();
        actors.push_back(actor);

        SortByLayer();
    }

    void ObjectManager::Remove(int EntityId)
    {
        auto it = std::find_if(actors.begin(), actors.end(), [EntityId](GameObject *obj)
                               { return obj->GetObjectId() == EntityId; });
        if (it != actors.end())
        {
            delete *it;
            actors.erase(it);
        }
    }

    GameObject *ObjectManager::FindById(int EntityId)
    {
        auto it = std::find_if(actors.begin(), actors.end(), [EntityId](GameObject *obj)
                               { return obj->GetObjectId() == EntityId; });
        if (it != actors.end())
        {
            return *it;
        }
        else
        {
            return nullptr;
        }
    }

    GameObject *ObjectManager::FindByUniqueName(std::string UniqueName)
    {
        auto it = std::find_if(actors.begin(), actors.end(), [UniqueName](GameObject *obj)
                               { return obj->GetObjectName() == UniqueName; });
        if (it != actors.end())
        {
            return *it;
        }
        else
        {
            return nullptr;
        }
    }

    void ObjectManager::SortByLayer()
    {
        std::sort(actors.begin(), actors.end(), [](GameObject *a, GameObject *b)
                  { return a->GetLayerId() < b->GetLayerId(); });
    }

    void ObjectManager::Clear()
    {

        while (!actors.empty())
        {
            delete actors.back();
            actors.pop_back();
        }

        nextEntityId = 0;

        std::cout << "Object manager cleared" << std::endl;
    }

    ObjectManager::~ObjectManager()
    {
        Clear();
        std::cout << "Object manager destroyed" << std::endl;
    }

}
