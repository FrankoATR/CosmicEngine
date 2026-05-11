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
        BodyManager();
        ~BodyManager();
        BodyManager(const BodyManager &) = delete;
        BodyManager &operator=(const BodyManager &) = delete;

        std::vector<GameBodyObject *> bodys;
        std::vector<int> toDelete;
        GameGridCollisions *GridArea;
        int nextEntityId;

    public:
        static BodyManager &GetInstance();

        glm::vec2 GetGridPosition();
        void SetGridPosition(glm::vec2 NewPosition);

        void Init();
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