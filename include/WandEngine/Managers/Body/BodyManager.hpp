#ifndef BODYMANAGER_HPP
#define BODYMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>

namespace WandEngine
{

    class GameBodyObject;
    class GameObject;
    class GameGridCollisions;

    class BodyManager
    {
    private:
        std::vector<GameBodyObject *> bodys;
        std::vector<int> toDelete;
        GameGridCollisions *GridArea;
        int nextEntityId;

        BodyManager();
        ~BodyManager();
        BodyManager(const BodyManager &) = delete;
        BodyManager &operator=(const BodyManager &) = delete;

    public:
        static BodyManager &GetInstance()
        {
            static BodyManager instance;
            return instance;
        }

        glm::vec2 GetGridPosition();
        void SetGridPosition(glm::vec2 NewPosition);

        void Update();
        void Draw();
        void Add(GameBodyObject *body);

        std::vector<GameBodyObject *> FindAllByParent(GameObject* Parent);

        void Remove(int entityId);
        void Clear();

        void SetNewGridArea(GameGridCollisions*);
        bool RectToRectCollisionBody(GameBodyObject *body_1, GameBodyObject *body_2);
    };

}

#endif // BODYMANAGER_HPP