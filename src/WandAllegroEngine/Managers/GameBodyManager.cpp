#include "GameBodyManager.hpp"
#include "../Models/GameObject.hpp"

GameBodyManager::GameBodyManager()
{

    std::vector<GameGridCollisions> gameGrids;

    for (auto &grid : gameGrids)
    {
        // gameGridCollisions.push_back(GameGridCollisions(Vec2(0, 0), 9, 9, 100));
    }

    gameGridCollisions.push_back(GameGridCollisions(Vec2(0, 0), 10, 18, 100));
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

    /*
        for (const auto &body : bodys)
    {
        for (const auto &body2 : bodys)
        {
            if (body != body2)
            {
                if (RectToRectCollisionBody(body, body2))
                {
                    //std::cout << "COLLISION" << std::endl;   //VARIAS COLISIONES SE ESTAN BORRANDO A LA VEZ, CUANDO SOLO TOCO 1
                    body->GetParent()->OnCollision(body2->GetParent());
                    body2->GetParent()->OnCollision(body->GetParent());
                }
            }
        }
    }
    */

    for (auto &grid : gameGridCollisions)
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
    for (auto &grid : gameGridCollisions)
    {
        grid.DrawCells();
    }
    for (auto body : bodys)
    {
        body->DrawBody();
    }
}

void GameBodyManager::Add(GameObject *obj, Vec2 Position, Vec2 Size)
{
    if (obj)
    {
        GameBodyObject* body = new GameBodyObject(obj, obj->GetPosition(), obj->GetSize());
        body->SetObjectId(obj->GetObjectId());
        bodys.push_back(body);
    }
}

void GameBodyManager::Remove(int entityId)
{
    auto it = std::find_if(bodys.begin(), bodys.end(), [entityId](GameBodyObject *body)
                           { return body->GetObjectId() == entityId; });
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
    // gameGridCollisions.clear(); Pensar si hacer un Manager para grid y otro para colisiones

    for (const auto &body : bodys)
    {
        delete body;
    }
    bodys.clear();
}