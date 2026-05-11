#ifndef GAMEBODYMANAGER_HPP
#define GAMEBODYMANAGER_HPP

#include <algorithm>
#include "../Collisions/GameGridCollisions.hpp"

class GameBodyManager {
private:
    std::vector<GameBodyObject*> bodys;
    std::vector<GameGridCollisions > gameGridCollisions;
public:
    GameBodyManager();
    ~GameBodyManager();

    void Update();
    void Draw();
    void Add(GameObject *obj, Vec2 Position, Vec2 Size);
    void Remove(int entityId);
    void Clear();

    bool RectToRectCollisionBody(GameBodyObject* body_1, GameBodyObject* body_2);

};

#endif // GAMEBODYMANAGER_HPP