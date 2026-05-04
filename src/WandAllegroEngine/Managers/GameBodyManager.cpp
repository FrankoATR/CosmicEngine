#include "GameBodyManager.h"
#include "../Models/GameObject.h"

GameBodyManager::GameBodyManager()
{

    std::vector<GameGridCollisions> gameGrids;

    for (auto &grid : gameGrids)
    {
        // gameGridCollisions.push_back(GameGridCollisions(Vec2(0, 0), 9, 9, 100));
    }

    gameGridCollisions.push_back(GameGridCollisions(Vec2(0, 0), 9, 12, 100));
    // gameGridCollisions.push_back(GameGridCollisions(Vec2(500, 700), 5, 2, 100));
}

GameBodyManager::~GameBodyManager()
{
    Clear();
}

void GameBodyManager::Update()
{
    std::vector<int> toRemove;

    for (const auto &body : bodys)
    {
        if (body->GetParent()->GetAliveInGameManager())
        {
            body->SetPosition(body->GetParent()->GetPosition());
            for (auto &grid : gameGridCollisions)
            {
                grid.AddObject(body);
            }
        }
        else
        {
            toRemove.push_back(body->GetObjectId());
        }
    }

    for (int id : toRemove)
    {
        Remove(id);
    }

    for (auto& grid : gameGridCollisions)
    {
        grid.Find_collision_grid();
        grid.ClearGrid();
    }

    // gameGridCollisions->UpdatePositions();
}

bool GameBodyManager::RectToRectCollisionBody(GameBodyObject *body1, GameBodyObject *body2)
{
    return (body1->GetPosition().x < body2->GetPosition().x + body2->GetSize().x &&
            body1->GetPosition().x + body1->GetSize().x > body2->GetPosition().x &&
            body1->GetPosition().y < body2->GetPosition().y + body2->GetSize().y &&
            body1->GetPosition().y + body1->GetSize().y > body2->GetPosition().y);
}

void GameBodyManager::Draw()
{
    for (auto& grid : gameGridCollisions)
    {
        grid.DrawCells();
    }
    for (auto body : bodys)
    {
        body->DrawBody();
    }
}

void GameBodyManager::Add(GameBodyObject *body)
{
    if(body->GetParent()){
        body->SetObjectId(body->GetParent()->GetObjectId());
        bodys.push_back(body);
    }
}

void GameBodyManager::Remove(int entityId)
{
    auto it = std::find_if(bodys.begin(), bodys.end(), [entityId](GameBodyObject *obj)
                           { return obj->GetObjectId() == entityId; });
    if (it != bodys.end())
    {
        delete *it;
        bodys.erase(it);
    }
}

void GameBodyManager::Clear()
{
    for (auto &grid : gameGridCollisions)
    {
        grid.ClearGrid();
    }
    //gameGridCollisions.clear(); Pensar si hacer un Manager para grid y otro para colisiones

    for (const auto& body : bodys)
    {
        delete body;
    }
    bodys.clear();
}