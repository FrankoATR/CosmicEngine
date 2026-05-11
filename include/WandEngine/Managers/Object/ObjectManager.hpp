#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP

#include "../../Utils/Configurations.hpp"
#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <string>

namespace WandEngine
{
    class Object;

    class ObjectManager
    {
    private:
        ObjectManager();
        ~ObjectManager();
        ObjectManager(const ObjectManager &) = delete;
        ObjectManager &operator=(const ObjectManager &) = delete;

        std::vector<Object *> objects;
        std::vector<int> toDelete;

        int nextEntityId;

    public:
        static ObjectManager &GetInstance();

        void Init();

        void Draw();
        void Update(float deltaTime);
        
        void Add(Object *obj);
        void Remove(int entityId);

        Object *FindById(int EntityId);
        std::vector<Object *> FindByClassName(std::string className);
        std::vector<Object *> GetAll();
        void Clear();

        #if defined(GAME_2D_CONFIGURATION)
            std::vector<Object *> FindByPosition(glm::vec2 position);
            std::vector<Object *> FindByMousePosition();
            std::vector<Object *> FindByLayer(int layerId);
            std::vector<Object *> FindByArea(glm::vec2 point1, glm::vec2 point2);
            void SortByLayer();

        #elif defined(GAME_3D_CONFIGURATION)
            std::vector<Object *> FindByPosition(glm::vec3 position);
            std::vector<Object *> FindByVolume(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);
            std::vector<Object *> FindByViewRayCast();

        #else
            #error "[ObjectManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
        #endif

    };

}

#endif // OBJECTMANAGER_HPP