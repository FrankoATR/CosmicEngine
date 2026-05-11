#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <vector>
#include <string>
#include <algorithm>

class GameEvent;
class GameObject;

class EventManager
{
private:
    EventManager() = default;
    ~EventManager();
    EventManager(const EventManager &) = delete;
    EventManager &operator=(const EventManager &) = delete;

    std::vector<GameEvent *> gameEvents;

public:
    static EventManager &GetInstance()
    {
        static EventManager instance;
        return instance;
    }
    void Add(GameEvent *GameEvent);
    void Remove(GameEvent *GameEvent);
    void Clear();
    void Notify(GameObject *obj, const std::string &event);
};

#endif // EVENTMANAGER_HPP
