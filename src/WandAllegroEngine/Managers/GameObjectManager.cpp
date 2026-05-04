#include "GameObjectManager.h"
#include "../Models/GameObject.h"

GameObjectManager::GameObjectManager(){
    
}

GameObjectManager::~GameObjectManager(){
    Clear();
}



void GameObjectManager::Update(float deltaTime) {
    std::vector<int> toRemove;

    for (auto actor : actors) {
        if (actor->GetAliveInGameManager()) {
            actor->Update(deltaTime);
        } else {
            toRemove.push_back(actor->GetObjectId());
        }
    }

    for (int id : toRemove) {
        Remove(id);
    }
}

void GameObjectManager::Draw() {
    for (auto actor : actors) {
        actor->Draw();
    }
}

void GameObjectManager::Add(GameObject* actor) {
    actor->SetObjectId(nextEntityId);
    nextEntityId++;
    actors.push_back(actor);

    std::sort(actors.begin(), actors.end(), [](GameObject* a, GameObject* b) {
        return a->GetLayerId() < b->GetLayerId();
    });

}

void GameObjectManager::Remove(int entityId) {
    auto it = std::find_if(actors.begin(), actors.end(), [entityId](GameObject* obj) {
        return obj->GetObjectId() == entityId;
    });
    if (it != actors.end()) {
        delete *it;
        actors.erase(it);
    }
}

void GameObjectManager::Clear() {
    for (auto actor : actors) {
        delete actor;
    }
    actors.clear();
    nextEntityId = 0;
}
