#include "BodyManager.hpp"
#include "../Models/GameObject.hpp"
#include "../Collisions/GameGridCollisions.hpp"

namespace WandEngine
{

    BodyManager::BodyManager()
    {
        this->GridArea = new GameGridCollisions(Vec2(0, 0), 10, 18, 100);
    }

    Vec2 BodyManager::GetGridPosition()
    {
        this->GridArea->GetPosition();
    }
    void BodyManager::SetGridPosition(Vec2 NewPosition)
    {
        this->GridArea->SetPosition(NewPosition);
    }

    void BodyManager::Update()
    {
        std::vector<int> toRemove;

        for (const auto &body : bodys)
        {
            if (body->GetParent()->GetAliveInGameManager())
            {
                body->SetPosition(body->GetParent()->GetPosition());
                GridArea->AddObject(body);
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

        GridArea->Find_collision_grid();
        GridArea->ClearGrid();
    }

    bool BodyManager::RectToRectCollisionBody(GameBodyObject *body1, GameBodyObject *body2)
    {
        return (body1->GetPosition().x < body2->GetPosition().x + body2->GetSize().x &&
                body1->GetPosition().x + body1->GetSize().x > body2->GetPosition().x &&
                body1->GetPosition().y < body2->GetPosition().y + body2->GetSize().y &&
                body1->GetPosition().y + body1->GetSize().y > body2->GetPosition().y);
    }

    void BodyManager::Draw()
    {

        GridArea->DrawCells();

        for (auto body : bodys)
        {
            body->DrawBody();
        }
    }

    void BodyManager::Add(GameObject *obj, Vec2 Position, Vec2 Size)
    {
        if (obj)
        {
            GameBodyObject *body = new GameBodyObject(obj, obj->GetPosition(), obj->GetSize());
            body->SetObjectId(obj->GetObjectId());
            bodys.push_back(body);
        }
    }

    void BodyManager::Remove(int entityId)
    {
        auto it = std::find_if(bodys.begin(), bodys.end(), [entityId](GameBodyObject *body)
                               { return body->GetObjectId() == entityId; });
        if (it != bodys.end())
        {
            delete *it;
            bodys.erase(it);
        }
    }

    void BodyManager::Clear()
    {

        GridArea->ClearGrid();

        while (!bodys.empty())
        {
            delete bodys.back();
            bodys.pop_back();
        }
        std::cout << "Body manager cleared" << std::endl;
    }

    BodyManager::~BodyManager()
    {
        Clear();
        std::cout << "Body manager destroyed" << std::endl;
    }
}
