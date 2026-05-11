#ifndef BODYMANAGER_HPP
#define BODYMANAGER_HPP

#include <algorithm>
#include <vector>
#include "../Interfaces/Definitions.hpp"

namespace WandEngine
{

    class GameBodyObject;
    class GameObject;
    class GameGridCollisions;

    class BodyManager
    {
    private:
        std::vector<GameBodyObject *> bodys;
        GameGridCollisions *GridArea;

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

        Vec2 GetGridPosition();
        void SetGridPosition(Vec2 NewPosition);

        void Update();
        void Draw();
        void Add(GameObject *obj, Vec2 Position, Vec2 Size);
        void Remove(int entityId);
        void Clear();

        bool RectToRectCollisionBody(GameBodyObject *body_1, GameBodyObject *body_2);
    };

}

#endif // BODYMANAGER_HPP