#include "ObjectManager.hpp"
#include "../Body/BodyManager.hpp"
#include "../Input/InputManager.hpp"
#include "../Camera/CameraManager.hpp"
#include "../../Models/GameObject.hpp"

#include <iostream>

namespace WandEngine
{
    ObjectManager &ObjectManager::GetInstance()
    {
        static ObjectManager instance;
        return instance;
    }

    ObjectManager::ObjectManager()
    {
        std::cout << "Object manager created" << std::endl;
    }

    ObjectManager::~ObjectManager()
    {
        std::cout << "Object manager destroyed" << std::endl;
    }

    void ObjectManager::Init()
    {
        this->nextEntityId = 0;
        std::cout << "Object manager initialized" << std::endl;
    }

    void ObjectManager::Update(float deltaTime)
    {
        for(auto ID : toDelete)
        {
            Remove(ID);
        }
        toDelete.clear();

        for (auto actor : objects)
        {
            if (actor->GetAliveInGameManager())
            {
                if (CameraManager::GetInstance().IsObjectInsideCameraArea(actor))
                {
                    actor->SetVisible(true);
                }
                else
                {
                    actor->SetVisible(false);
                }

                actor->UpdateLastPosition();
                actor->Update(deltaTime);
                actor->UpdatePosition(deltaTime);
                
            }
            else
            {
                toDelete.push_back(actor->GetID());
            }
        }


    }

    void ObjectManager::Draw()
    {
        for (auto actor : objects)
        {
            if (actor->GetVisible())
            {
                actor->Draw();
            }
        }
    }

    void ObjectManager::Add(GameObject *actor)
    {
        actor->SetID(nextEntityId++);
        actor->Init();
        objects.push_back(actor);

        SortByLayer();
    }

    void ObjectManager::Remove(int EntityId)
    {
        auto it = std::find_if(objects.begin(), objects.end(), [EntityId](GameObject* obj) {
            return obj && obj->GetID() == EntityId;
        });

        if (it != objects.end()) {
            GameObject* tmp = *it;
            for(auto **copies : tmp->pointer_copies)
            {
                if(*copies == tmp)
                {
                    (*copies) = nullptr;
                }
            }
            objects.erase(it);
            delete tmp;
        }
    }

    GameObject *ObjectManager::FindById(int EntityId)
    {
        auto it = std::find_if(objects.begin(), objects.end(), [EntityId](GameObject *obj)
                               { return obj && obj->GetID() == EntityId; });
        if (it != objects.end())
        {
            return *it;
        }

        return nullptr;
    }

    std::vector<GameObject *> ObjectManager::FindByClassName(std::string className)
    {
        std::vector<GameObject *> tmpObjts;

        for (auto &actor : objects)
        {
            if(actor && actor->GetClassName() == className)
            {
                tmpObjts.push_back(actor);
            }
        }

        return tmpObjts;
    }

    std::vector<GameObject *> ObjectManager::FindByPosition(glm::vec2 Position)
    {
        std::vector<GameObject *> tmpObjts;

        for (auto &actor : objects)
        {
            if (actor && Position.x >= actor->GetPosition().x && Position.x <= actor->GetPosition().x + actor->GetSize().x && Position.y >= actor->GetPosition().y && Position.y <= actor->GetPosition().y + actor->GetSize().y)
            {
                tmpObjts.push_back(actor);
            }
        }
        return tmpObjts;
    }

    std::vector<GameObject*> ObjectManager::FindByArea(glm::vec2 point1, glm::vec2 point2)
    {
        std::vector<GameObject*> tmpObjts;

        float xMin = std::min(point1.x, point2.x);
        float xMax = std::max(point1.x, point2.x);
        float yMin = std::min(point1.y, point2.y);
        float yMax = std::max(point1.y, point2.y);

        for (auto& actor : objects)
        {
            if (!actor)
                continue;
                
            glm::vec2 pos = actor->GetPosition();
            glm::vec2 size = actor->GetSize();

            float actorLeft   = pos.x;
            float actorRight  = pos.x + size.x;
            float actorTop    = pos.y;
            float actorBottom = pos.y + size.y;

            if (actorRight >= xMin && actorLeft <= xMax &&
                actorBottom >= yMin && actorTop <= yMax)
            {
                tmpObjts.push_back(actor);
            }
        }

        return tmpObjts;
    }

    std::vector<GameObject *> ObjectManager::FindByMousePosition()
    {
        std::vector<GameObject *> tmpObjts;
        glm::vec2 MousePosition = InputManager::GetInstance().GetMousePosition();

        for (auto &actor : objects)
        {
            if (actor && MousePosition.x >= actor->GetPosition().x && MousePosition.x <= actor->GetPosition().x + actor->GetSize().x && MousePosition.y >= actor->GetPosition().y && MousePosition.y <= actor->GetPosition().y + actor->GetSize().y)
            {
                tmpObjts.push_back(actor);
            }
        }
        return tmpObjts;
    }

    std::vector<GameObject *> ObjectManager::FindByLayer(int LayerId)
    {
        std::vector<GameObject *> tmpObjts;
        for (auto &actor : objects)
        {
            if (actor && actor->GetLayerId() == LayerId)
            {
                tmpObjts.push_back(actor);
            }
        }
        return tmpObjts;
    }

    std::vector<GameObject *> ObjectManager::GetAll()
    {
        return this->objects;
    }

    void ObjectManager::SortByLayer()
    {
        std::sort(objects.begin(), objects.end(), [](GameObject *a, GameObject *b)
                  { return (a->GetLayerId() < b->GetLayerId()) ||
                           (a->GetLayerId() == b->GetLayerId() && a->GetID() < b->GetID()); });
    }

    void ObjectManager::Clear()
    {

        while (!objects.empty())
        {
            GameObject* obj = objects.back();
            if(obj)
            {
                for(auto **copies : obj->pointer_copies)
                {
                    (*copies) = nullptr;
                }
                delete obj;
            }
            objects.pop_back();
        }
        objects.clear();

        toDelete.clear();

        nextEntityId = 0;
        
        std::cout << "Object manager cleared" << std::endl;
    }


}
