#include "ObjectManager.hpp"
#include "BodyManager.hpp"
#include "../Models/GameObject.hpp"
#include "InputManager.hpp"
#include "CameraManager.hpp"

namespace WandEngine
{
    ObjectManager::ObjectManager() : nextEntityId(0)
    {
    }

    void ObjectManager::Update(float deltaTime)
    {
        std::vector<int> toRemove;

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

                if (actor->GetInsideGridArea())
                {
                    actor->Update(deltaTime);
                }
            }
            else
            {
                std::vector<GameBodyObject *> BodysToDestoy = BodyManager::GetInstance().FindAllByParent(actor);  // INEFICIENTE POR CADA DESTROY DE OBJ
                for(auto body : BodysToDestoy)
                {
                    body->Destroy();
                }
                toRemove.push_back(actor->GetObjectId());
            }
        }

        for (int id : toRemove)
        {
            Remove(id);
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
        actor->SetObjectId(nextEntityId++);
        actor->Init();
        objects.push_back(actor);

        SortByLayer();
    }

    void ObjectManager::Remove(int EntityId)
    {
        auto it = std::find_if(objects.begin(), objects.end(), [EntityId](GameObject *obj)
                               { return obj->GetObjectId() == EntityId; });
        if (it != objects.end())
        {
            delete *it;
            objects.erase(it);
        }
    }

    GameObject *ObjectManager::FindById(int EntityId)
    {
        auto it = std::find_if(objects.begin(), objects.end(), [EntityId](GameObject *obj)
                               { return obj->GetObjectId() == EntityId; });
        if (it != objects.end())
        {
            return *it;
        }
        else
        {
            return nullptr;
        }
    }

    GameObject *ObjectManager::FindByUniqueName(std::string UniqueName)
    {
        auto it = std::find_if(objects.begin(), objects.end(), [UniqueName](GameObject *obj)
                               { return obj->GetObjectName() == UniqueName; });
        if (it != objects.end())
        {
            return *it;
        }
        else
        {
            return nullptr;
        }
    }

    std::vector<GameObject *> ObjectManager::FindByPosition(WAND_VEC2 Position)
    {
        std::vector<GameObject *> ObjectsInPosition;

        for (auto &actor : objects)
        {
            if (Position.x >= actor->GetPosition().x && Position.x <= actor->GetPosition().x + actor->GetSize().x && Position.y >= actor->GetPosition().y && Position.y <= actor->GetPosition().y + actor->GetSize().y)
            {
                ObjectsInPosition.push_back(actor);
            }
        }
        return ObjectsInPosition;
    }

    std::vector<GameObject *> ObjectManager::FindByMousePosition()
    {
        std::vector<GameObject *> ObjectsInPosition;
        WAND_VEC2 MousePosition = InputManager::GetInstance().GetMousePosition();

        for (auto &actor : objects)
        {
            if (MousePosition.x >= actor->GetPosition().x && MousePosition.x <= actor->GetPosition().x + actor->GetSize().x && MousePosition.y >= actor->GetPosition().y && MousePosition.y <= actor->GetPosition().y + actor->GetSize().y)
            {
                ObjectsInPosition.push_back(actor);
            }
        }
        return ObjectsInPosition;
    }

    std::vector<GameObject *> ObjectManager::FindByLayer(int LayerId)
    {
        std::vector<GameObject *> ObjectsInlayer;
        for (auto &actor : objects)
        {
            if (actor->GetLayerId() == LayerId)
            {
                ObjectsInlayer.push_back(actor);
            }
        }
        return ObjectsInlayer;
    }

    std::vector<GameObject *> ObjectManager::GetAll()
    {
        return this->objects;
    }

    void ObjectManager::SortByLayer()
    {
        std::sort(objects.begin(), objects.end(), [](GameObject *a, GameObject *b)
                  { return (a->GetLayerId() < b->GetLayerId()) ||
                           (a->GetLayerId() == b->GetLayerId() && a->GetObjectId() < b->GetObjectId()); });
    }

    void ObjectManager::Clear()
    {

        while (!objects.empty())
        {
            delete objects.back();
            objects.pop_back();
        }

        nextEntityId = 0;
        
        #ifndef NDEBUG
            std::cout << "Object manager cleared" << std::endl;
		#endif
    }

    ObjectManager::~ObjectManager()
    {
        #ifndef NDEBUG
            std::cout << "Object manager destroyed" << std::endl;
		#endif
    }

}
