#include "object_manager.hpp"
#include "../body/body_manager.hpp"
#include "../input/input_manager.hpp"
#include "../camera/camera_manager.hpp"
#include "../../models/object/object.hpp"
#include "../../utils/log.hpp"

namespace CosmicEngine
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

    void ObjectManager::init()
    {
        this->nextEntityId = 0;
        RUNTIME_INFO("Object manager initialized");
    }

    void ObjectManager::update(float deltaTime)
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
                #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
                    CameraManager::GetInstance().IsObjectInsideCameraArea(obj) ? obj->SetVisible(true) : obj->SetVisible(false);
                #endif

                obj->update(deltaTime);
                obj->UpdatePosition(deltaTime);
            }
            else
            {
                toDelete.push_back(obj->GetID());
            }
        }


    }

    void ObjectManager::draw()
    {
        for (auto obj : objects)
        {
            if (obj->GetVisible())
            {
                obj->draw();
            }
        }
    }

    void ObjectManager::Add(Object *obj)
    {
        obj->ID = nextEntityId++;
        obj->init();
        objects.push_back(obj);

        #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
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


    #if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
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


    #elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION

        std::vector<Object *> ObjectManager::FindByPosition(glm::vec3 position)
        {
            std::vector<Object *> tmpObjts;
        
            for (auto &obj : objects)
            {
                if (!obj)
                    continue;
        
                glm::vec3 objPos = obj->GetPosition();
                glm::vec3 objSize = obj->GetSize();
                glm::vec3 min = objPos - objSize * 0.5f;
                glm::vec3 max = objPos + objSize * 0.5f;
        
                if (
                    position.x >= min.x && position.x <= max.x &&
                    position.y >= min.y && position.y <= max.y &&
                    position.z >= min.z && position.z <= max.z
                )
                {
                    tmpObjts.push_back(obj);
                }
            }
        
            return tmpObjts;
        }

        
        bool ObjectManager::ObjectOnViewDirection(float maxDistance, glm::vec3 *hitPosition, glm::vec3 *lastFreePosition)
        {
            glm::vec3 rayDir = glm::normalize(CameraManager::GetInstance().GetViewDirection());
            glm::vec3 rayPos = CameraManager::GetInstance().GetPosition();
            glm::vec3 current_pos = rayPos;
            glm::vec3 lastFree_pos = current_pos;
            glm::ivec3 lastFreeBlock = glm::floor(current_pos);

            for (int i = 0; i < maxDistance * 10; ++i)
            {
                rayPos += rayDir * 0.1f;
                current_pos = rayPos;
        
                if (current_pos != lastFree_pos)
                {
                    auto objs = FindByPosition(current_pos);
                    if (!objs.empty()) {
                        if(hitPosition)
                        {
                            *hitPosition = objs[0]->GetPosition();
                        }
                        if(lastFreePosition)
                        {
                            *lastFreePosition = glm::floor(lastFree_pos + 0.5f);
                        }
                        return true;
                    }
        
                    lastFree_pos = current_pos;
                }
        
                if (glm::length(rayPos - CameraManager::GetInstance().GetPosition()) > maxDistance)
                    break;
            }
        
            return false;
        }
    
    
    

    #else
        #error "[ObjectManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
    #endif


}
