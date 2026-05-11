#ifndef GAMEOBJECTMANAGER_HPP
#define GAMEOBJECTMANAGER_HPP

#include <algorithm>
#include <list>
#include <vector>

class GameObject;

class GameObjectManager {
private:
    std::vector<GameObject*> actors;
    int nextEntityId;
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

#endif // GAMEOBJECTMANAGER_HPP