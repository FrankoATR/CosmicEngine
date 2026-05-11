#include "BodyManager.hpp"
#include "../Models/GameObject.hpp"
#include "../Collisions/GameGridCollisions.hpp"

namespace WandEngine
{

    BodyManager::BodyManager() : nextEntityId(0)
    {
        this->GridArea = nullptr;
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

    WAND_VEC2 BodyManager::GetGridPosition()
    {
        if (GridArea)
        {
            return this->GridArea->GetPosition();
        }
        else
        {
            return WAND_VEC2(0, 0);
        }
    }
    void BodyManager::SetGridPosition(WAND_VEC2 NewPosition)
    {
        if (GridArea)
        {
            this->GridArea->SetPosition(NewPosition);
        }
    }

    void BodyManager::Update()
    {
        std::vector<int> toRemove;

        for (const auto &body : bodys)
        {
            if (body->GetAliveInGameManager())
            {
                //body->SetPosition(body->GetParent()->GetPosition());
                if (GridArea)
                {
                    GridArea->AddObject(body);
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
            //GridArea->DrawCells();
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
        if (body)
        {
            body->SetObjectId(nextEntityId++);
            bodys.push_back(body);
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

        if (GridArea)
        {
            GridArea->ClearGrid();
            delete GridArea;
            GridArea = nullptr;
        }

        while (!bodys.empty())
        {
            delete bodys.back();
            bodys.pop_back();
        }
        
        #ifndef NDEBUG
            std::cout << "Body manager cleared" << std::endl;
		#endif
    }

    BodyManager::~BodyManager()
    {
        #ifndef NDEBUG
            std::cout << "Body manager destroyed" << std::endl;
		#endif
    }
}
