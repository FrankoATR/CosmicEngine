#include "EventManager.hpp"
#include "../Interfaces/GameEvent.hpp"
#include "../Models/GameObject.hpp"

void EventManager::Add(GameEvent *GameEvent)
{
    gameEvents.push_back(GameEvent);
}

void EventManager::Remove(GameEvent *GameEvent)
{
    gameEvents.erase(std::remove(gameEvents.begin(), gameEvents.end(), GameEvent), gameEvents.end());
}

void EventManager::Notify(GameObject *obj, const std::string &event)
{
    for (GameEvent *GameEvent : gameEvents)
    {
        GameEvent->OnNotify(obj, event);
    }
}

void EventManager::Clear()
{
    for (GameEvent *event : gameEvents)
    {
        delete event;
    }
    gameEvents.clear();

    std::cout << "Event manager clear" << std::endl;
}

EventManager::~EventManager()
{
    Clear();
    std::cout << "Event manager destroyed" << std::endl;
}