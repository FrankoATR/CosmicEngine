#ifndef OBJECTMANAGER_HPP
#define OBJECTMANAGER_HPP

#include <algorithm>
#include <list>
#include <vector>
#include <string>

namespace WandEngine
{
    class GameObject;

    class ObjectManager
    {
    private:
        std::vector<GameObject *> actors;
        int nextEntityId;

        ObjectManager();
        ~ObjectManager();
        ObjectManager(const ObjectManager &) = delete;
        ObjectManager &operator=(const ObjectManager &) = delete;

    public:
        static ObjectManager &GetInstance()
        {
            static ObjectManager instance;
            return instance;
        }

        void Update(float deltaTime);
        void Draw();
        void Add(GameObject *actor);
        void Remove(int EntityId);
        GameObject *FindById(int EntityId);
        GameObject *FindByUniqueName(std::string UniqueName);
        std::vector<GameObject *> FindByLayer();
        void SortByLayer();
        void Clear();
    };

}

#endif // OBJECTMANAGER_HPP