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

        WAND_VEC2 GetGridPosition();
        void SetGridPosition(WAND_VEC2 NewPosition);

        void Update();
        void Draw();
        void Add(GameBodyObject *body);
        void Remove(int entityId);
        void Clear();

        void SetNewGridArea(GameGridCollisions*);
        bool RectToRectCollisionBody(GameBodyObject *body_1, GameBodyObject *body_2);
    };

}

#endif // BODYMANAGER_HPP