#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP

#include <glm/glm.hpp>
#include <algorithm>
#include <vector>
#include <string>

namespace WandEngine
{
    class GameObject;

    class ObjectManager
    {
    private:
        ObjectManager();
        ~ObjectManager();
        ObjectManager(const ObjectManager &) = delete;
        ObjectManager &operator=(const ObjectManager &) = delete;

        std::vector<GameObject *> objects;
        std::vector<int> toDelete;

        int nextEntityId;

    public:
        static ObjectManager &GetInstance();

        void Init();

        void Update(float deltaTime);
        void Draw();
        void Add(GameObject *actor);
        void Remove(int EntityId);

        GameObject *FindById(int EntityId);
        std::vector<GameObject *> FindByClassName(std::string className);
        std::vector<GameObject *> FindByPosition(glm::vec2 Position);
        std::vector<GameObject *> FindByArea(glm::vec2 point1, glm::vec2 point2);
        std::vector<GameObject *> FindByMousePosition();
        std::vector<GameObject *> FindByLayer(int LayerId);
        std::vector<GameObject *> GetAll();
        void SortByLayer();
        void Clear();
    };

}

#endif // OBJECTMANAGER_HPP