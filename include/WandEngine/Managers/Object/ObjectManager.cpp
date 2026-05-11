#include "ObjectManager.hpp"
#include "../Body/BodyManager.hpp"
#include "../Input/InputManager.hpp"
#include "../Camera/CameraManager.hpp"
#include "../../Models/Object/Object.hpp"
#include "../../Utils/Log.hpp"

namespace WandEngine
{
    ObjectManager &ObjectManager::GetInstance()
    {
        static ObjectManager instance;
        return instance;
    }

    ObjectManager::ObjectManager()
    {
        RUNTIME_INFO("Object manager created");
    }

    ObjectManager::~ObjectManager()
    {
        RUNTIME_INFO("Object manager destroyed");
    }

    void ObjectManager::Init()
    {
        this->nextEntityId = 0;
        RUNTIME_INFO("Object manager initialized");
    }

    void ObjectManager::Update(float deltaTime)
    {
        for(auto ID : toDelete)
        {
            Remove(ID);
        }
        toDelete.clear();

        for (auto obj : objects)
        {
            if (obj->IsAlive())
            {
                #if defined(GAME_2D_CONFIGURATION)
                    CameraManager::GetInstance().IsObjectInsideCameraArea(obj) ? obj->SetVisible(true) : obj->SetVisible(false);
                #endif

                obj->Update(deltaTime);
                obj->UpdatePosition(deltaTime);
            }
            else
            {
                toDelete.push_back(obj->GetID());
            }
        }


    }

    void ObjectManager::Draw()
    {
        for (auto obj : objects)
        {
            if (obj->GetVisible())
            {
                obj->Draw();
            }
        }
    }

    void ObjectManager::Add(Object *obj)
    {
        obj->ID = nextEntityId++;
        obj->Init();
        objects.push_back(obj);

        #if defined(GAME_2D_CONFIGURATION)
            SortByLayer();
        #endif
    }

    void ObjectManager::Remove(int entityId)
    {
        auto it = std::find_if(objects.begin(), objects.end(), [entityId](Object* obj) {
            return obj && obj->GetID() == entityId;
        });

        if (it != objects.end()) {
            Object* tmp = *it;
            tmp->unReferenceAll();
            objects.erase(it);
            delete tmp;
        }
    }

    Object *ObjectManager::FindById(int entityId)
    {
        auto it = std::find_if(objects.begin(), objects.end(), [entityId](Object *obj)
                               { return obj && obj->GetID() == entityId; });
        if (it != objects.end())
        {
            return *it;
        }

        return nullptr;
    }

    std::vector<Object *> ObjectManager::FindByClassName(std::string className)
    {
        std::vector<Object *> tmpObjts;

        for (auto &obj : objects)
        {
            if(obj && obj->GetClassName() == className)
            {
                tmpObjts.push_back(obj);
            }
        }

        return tmpObjts;
    }


    std::vector<Object *> ObjectManager::GetAll()
    {
        return this->objects;
    }


    void ObjectManager::Clear()
    {

        while (!objects.empty())
        {
            Object* obj = objects.back();
            if(obj)
            {
                obj->unReferenceAll();
                delete obj;
            }
            objects.pop_back();
        }
        objects.clear();

        toDelete.clear();

        nextEntityId = 0;
        
        RUNTIME_INFO("Object manager cleared");
    }


    #if defined(GAME_2D_CONFIGURATION)
        std::vector<Object *> ObjectManager::FindByPosition(glm::vec2 position)
        {
            std::vector<Object *> tmpObjts;

            for (auto &obj : objects)
            {
                if (!obj)
                    continue;

                if (obj && position.x >= obj->GetPosition().x && position.x <= obj->GetPosition().x + obj->GetSize().x && position.y >= obj->GetPosition().y && position.y <= obj->GetPosition().y + obj->GetSize().y)
                {
                    tmpObjts.push_back(obj);
                }
            }
            return tmpObjts;
        }

        std::vector<Object*> ObjectManager::FindByArea(glm::vec2 point1, glm::vec2 point2)
        {
            std::vector<Object*> tmpObjts;

            float xMin = std::min(point1.x, point2.x);
            float xMax = std::max(point1.x, point2.x);
            float yMin = std::min(point1.y, point2.y);
            float yMax = std::max(point1.y, point2.y);

            for (auto& obj : objects)
            {
                if (!obj)
                    continue;
                    
                glm::vec2 pos = obj->GetPosition();
                glm::vec2 size = obj->GetSize();

                float objLeft   = pos.x;
                float objRight  = pos.x + size.x;
                float objTop    = pos.y;
                float objBottom = pos.y + size.y;

                if (objRight >= xMin && objLeft <= xMax &&
                    objBottom >= yMin && objTop <= yMax)
                {
                    tmpObjts.push_back(obj);
                }
            }

            return tmpObjts;
        }

        std::vector<Object *> ObjectManager::FindByMousePosition()
        {
            std::vector<Object *> tmpObjts;
            glm::vec2 MousePosition = InputManager::GetInstance().GetMousePosition();

            for (auto &obj : objects)
            {
                if (!obj)
                    continue;

                if (obj && MousePosition.x >= obj->GetPosition().x && MousePosition.x <= obj->GetPosition().x + obj->GetSize().x && MousePosition.y >= obj->GetPosition().y && MousePosition.y <= obj->GetPosition().y + obj->GetSize().y)
                {
                    tmpObjts.push_back(obj);
                }
            }
            return tmpObjts;
        }

        std::vector<Object *> ObjectManager::FindByLayer(int layerId)
        {
            std::vector<Object *> tmpObjts;
            for (auto &obj : objects)
            {
                if (!obj)
                continue;
                
                if (obj && obj->GetLayerId() == layerId)
                {
                    tmpObjts.push_back(obj);
                }
            }
            return tmpObjts;
        }

        void ObjectManager::SortByLayer()
        {
            std::sort(objects.begin(), objects.end(), [](Object *a, Object *b)
                    { 
                        return (a->GetLayerId() < b->GetLayerId()) ||
                            (a->GetLayerId() == b->GetLayerId() && a->GetID() < b->GetID()); });
        }


    #elif defined(GAME_3D_CONFIGURATION)


    #else
        #error "[ObjectManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif


}
