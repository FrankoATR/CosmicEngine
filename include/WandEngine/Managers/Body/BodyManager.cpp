#include "BodyManager.hpp"
#include "../../Models/GameBodyObject.hpp"
#include "../../Models/GameObject.hpp"

#include "../../Collisions/GameGridCollisions.hpp"
#include <algorithm>
#include <iostream>

namespace WandEngine
{
    BodyManager &BodyManager::GetInstance()
    {
        static BodyManager instance;
        return instance;
    }

    BodyManager::BodyManager()
    {
        std::cout << "Body manager created" << std::endl;
    }

    BodyManager::~BodyManager()
    {
        std::cout << "Body manager destroyed" << std::endl;
    }


    void BodyManager::Init()
    {
        this->GridArea = nullptr;
        this->nextEntityId = 0;
        std::cout << "Body manager initialized" << std::endl;
    }



    void BodyManager::SetNewGridArea(GameGridCollisions * NewGridArea)
    {
        if(GridArea)
        {
            GridArea->ClearGrid();
            delete GridArea;
        }
        this->GridArea = NewGridArea;
    }

    glm::vec2 BodyManager::GetGridPosition()
    {
        if (GridArea)
        {
            return this->GridArea->GetPosition();
        }
        else
        {
            return glm::vec2(0.0f);
        }
    }
    void BodyManager::SetGridPosition(glm::vec2 NewPosition)
    {
        if (GridArea)
        {
            this->GridArea->SetPosition(NewPosition);
        }
    }

    void BodyManager::Update()
    {
        for(auto ID : toDelete)
        {
            Remove(ID);
        }
        toDelete.clear();

        for (auto &body : bodys)
        {
            if (body->GetParent()->GetAliveInGameManager() && body->GetAliveInGameManager())
            {
                if (GridArea)
                {
                    body->UpdatePosition();
                    GridArea->AddObject(body);
                }
            }
            else
            {
                toDelete.push_back(body->GetID());
            }
        }

        if (GridArea)
        {
            GridArea->Find_collision_grid();
            GridArea->ClearGrid();
        }

    }

    void BodyManager::Draw()
    {

        if (GridArea)
        {
            GridArea->DrawCells();
        }

        for (auto body : bodys)
        {
            if(body->GetParent()->GetVisible())
            {
                body->DrawBody();
            }
        }
    }

    void BodyManager::Add(GameBodyObject *body)
    {
        if (body && body->GetParent())
        {
            body->SetID(nextEntityId++);
            bodys.push_back(body);
        }
        else
        {
            delete body;
            std::cerr << "Body needs to have a parent" << std::endl;
        }
    }

    std::vector<GameBodyObject *> BodyManager::FindAllByParent(GameObject* Parent)  // tal vez ordenarlos y buscar por binary search es mejor ?
    {
        std::vector<GameBodyObject *> bodys_found;
        for (auto body : bodys)
        {
            if (body->GetParent() == Parent)
            {
                bodys_found.push_back(body);
            }
        }

        return bodys_found;
    }

    void BodyManager::Remove(int entityId)
    {
        auto it = std::find_if(bodys.begin(), bodys.end(), [entityId](GameBodyObject* body) {
            return body && body->GetID() == entityId;
        });

        if (it != bodys.end()) {
            GameBodyObject* tmp = *it;
            bodys.erase(it);
            delete tmp;
        }
    }

    void BodyManager::Clear()
    {

        if (GridArea)
        {
            GridArea->ClearGrid();
            delete GridArea;
            GridArea = nullptr;
        }

        while (!bodys.empty())
        {
            GameBodyObject* body = bodys.back();
            if(body)
            {
                delete body;
            }
            bodys.pop_back();
        }
        bodys.clear();
        
        toDelete.clear();

        nextEntityId = 0;

        std::cout << "Body manager cleared" << std::endl;
    }


}
