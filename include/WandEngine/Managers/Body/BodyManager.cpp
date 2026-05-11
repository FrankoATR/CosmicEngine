#include "BodyManager.hpp"

#include "../../Models/Body/Body.hpp"
#include "../../Models/Object/Object.hpp"

#include "../../Utils/Log.hpp"
#include <algorithm>

#if defined(GAME_2D_CONFIGURATION)
    #include "../../Collisions/GameGridCollisions.hpp"

#elif defined(GAME_3D_CONFIGURATION)

#else
    #error "[BodyManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif

namespace WandEngine
{
    BodyManager &BodyManager::GetInstance()
    {
        static BodyManager instance;
        return instance;
    }

    BodyManager::BodyManager()
    {
        RUNTIME_INFO("Body manager created");
    }

    BodyManager::~BodyManager()
    {
        RUNTIME_INFO("Body manager destroyed");
    }

    void BodyManager::Add(Body *body)
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

    std::vector<Body *> BodyManager::FindAllByParent(Object* Parent)  // tal vez ordenarlos y buscar por binary search es mejor ?
    {
        std::vector<Body *> bodys_found;
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
        auto it = std::find_if(bodys.begin(), bodys.end(), [entityId](Body* body) {
            return body && body->GetID() == entityId;
        });

        if (it != bodys.end()) {
            Body* tmp = *it;
            bodys.erase(it);
            delete tmp;
        }
    }

    #if defined(GAME_2D_CONFIGURATION)

        void BodyManager::Init()
        {
            this->GridArea = nullptr;
            this->nextEntityId = 0;
            RUNTIME_INFO("Body manager initialized");
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
                    body->Draw();
                }
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
                if (body->GetParent()->IsAlive() && body->IsAlive())
                {
                    if (GridArea)
                    {
                        body->Update();
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

        void BodyManager::SetNewGridArea(GameGridCollisions * NewGridArea)
        {
            if(GridArea)
            {
                GridArea->ClearGrid();
                delete GridArea;
            }
            this->GridArea = NewGridArea;
        }

        void BodyManager::SetGridPosition(glm::vec2 NewPosition)
        {
            if (GridArea)
            {
                this->GridArea->SetPosition(NewPosition);
            }
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
                Body* body = bodys.back();
                if(body)
                {
                    delete body;
                }
                bodys.pop_back();
            }
            bodys.clear();
            
            toDelete.clear();
    
            nextEntityId = 0;
    
            RUNTIME_INFO("Body manager cleared");
        }
    
        


    #elif defined(GAME_3D_CONFIGURATION)
        void BodyManager::Init()
        {
            this->nextEntityId = 0;
            RUNTIME_INFO("Body manager initialized");
        }
        
        void BodyManager::Draw()
        {

            for (auto body : bodys)
            {
                if(body->GetParent()->GetVisible())
                {
                    body->Draw();
                }
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
                if (body->GetParent()->IsAlive() && body->IsAlive())
                {
                    body->Update();
                }
                else
                {
                    toDelete.push_back(body->GetID());
                }
            }

        }


        void BodyManager::Clear()
        {


            while (!bodys.empty())
            {
                Body* body = bodys.back();
                if(body)
                {
                    delete body;
                }
                bodys.pop_back();
            }
            bodys.clear();
            
            toDelete.clear();

            nextEntityId = 0;

            RUNTIME_INFO("Body manager cleared");
        }

        


    #else
        #error "[BodyManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif


}
