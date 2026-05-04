#include "GameBodyManager.h"
#include "../Models/GameObject.h"

GameBodyManager::GameBodyManager(){
    this->gameGridCollisions = new GameGridCollisions(Vec2(0, 0), 11, 19, 100);
}

GameBodyManager::~GameBodyManager(){
    Clear();
}


void GameBodyManager::Update() {
    std::vector<int> toRemove;

    for (auto body : bodys) {
        if(body->GetParent()->GetAliveInGameManager()){
            body->SetPosition(body->GetParent()->GetPosition());
        }
        else{
            toRemove.push_back(body->GetObjectId());
        }
    }

    for (int id : toRemove) {
        Remove(id);
    }

    gameGridCollisions->Find_collision_grid();
    gameGridCollisions->UpdatePositions();
    /*
    for (size_t i = 0; i < bodys.size(); ++i) {
        for (size_t j = i + 1; j < bodys.size(); ++j) {
            GameBodyObject* body1 = bodys[i];
            GameBodyObject* body2 = bodys[j];
            if (RectToRectCollisionBody(body1, body2)) {
                body1->GetParent()->OnCollision(body2->GetParent());
                body2->GetParent()->OnCollision(body1->GetParent());
            }
        }
    }
    */
}

bool GameBodyManager::RectToRectCollisionBody(GameBodyObject* body1, GameBodyObject* body2) {
    return (body1->GetPosition().x < body2->GetPosition().x + body2->GetSize().x &&
            body1->GetPosition().x + body1->GetSize().x > body2->GetPosition().x &&
            body1->GetPosition().y < body2->GetPosition().y + body2->GetSize().y &&
            body1->GetPosition().y + body1->GetSize().y > body2->GetPosition().y);
}


void GameBodyManager::Draw() {
    for (auto body : bodys) {
        gameGridCollisions->DrawCells();
        body->DrawBody();
    }
}


void GameBodyManager::Add(GameBodyObject* body) {
    body->SetObjectId(body->GetParent()->GetObjectId());
    bodys.push_back(body);
    gameGridCollisions->AddObject(body);
}


void GameBodyManager::Remove(int entityId) {
    auto it = std::find_if(bodys.begin(), bodys.end(), [entityId](GameBodyObject* obj) {
        return obj->GetObjectId() == entityId;
    });
    if (it != bodys.end()) {
        gameGridCollisions->RemoveObject(*it);
        delete *it;
        bodys.erase(it);
    }
}


void GameBodyManager::Clear() {
    for (auto body : bodys) {
        gameGridCollisions->RemoveObject(body);
        delete body;
    }
    bodys.clear();
}
