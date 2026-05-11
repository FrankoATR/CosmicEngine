#include "ObjectManager.hpp"
#include "../Models/GameObject.hpp"

WandEngine::ObjectManager::ObjectManager() : nextEntityId(0)
{

}

WandEngine::ObjectManager::~ObjectManager()
{
    Clear();
}

void WandEngine::ObjectManager::Update(float deltaTime)
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

void WandEngine::ObjectManager::Draw()
{
    for (auto actor : actors)
    {
        actor->Draw();
    }
}

void WandEngine::ObjectManager::Add(GameObject *actor)
{
    actor->SetObjectId(nextEntityId++);
    actor->Init();
    actors.push_back(actor);

    SortByLayer();
}

void WandEngine::ObjectManager::Remove(int entityId)
{
    auto it = std::find_if(actors.begin(), actors.end(), [entityId](GameObject *obj)
                           { return obj->GetObjectId() == entityId; });
    if (it != actors.end())
    {
        delete *it;
        actors.erase(it);
    }
}

void WandEngine::ObjectManager::SortByLayer()
{
    std::sort(actors.begin(), actors.end(), [](GameObject *a, GameObject *b)
              { return a->GetLayerId() < b->GetLayerId(); });
}

void WandEngine::ObjectManager::Clear()
{
    for (const auto &actor : actors)
    {
        delete actor;
    }
    actors.clear();
    nextEntityId = 0;
}
