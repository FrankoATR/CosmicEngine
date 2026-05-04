#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H

#include <algorithm>
#include <list>
#include <vector>

class GameObject;

class GameObjectManager {
private:
    std::vector<GameObject*> actors;
    int nextEntityId = 1;
public:
    GameObjectManager();
    ~GameObjectManager();

    void Update(float deltaTime);
    void Draw();
    void Add(GameObject* actor);
    void Remove(int entityId);
    void SortByLayer();
    void Clear();

};

#endif