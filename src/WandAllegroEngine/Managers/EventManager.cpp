#include "EventManager.hpp"
#include "../Interfaces/GameEvent.hpp"
#include "../Models/GameObject.hpp"

namespace WandEngine
{

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

        while(!gameEvents.empty())
        {
            delete gameEvents.back();
            gameEvents.pop_back();
        }

        std::cout << "Event manager cleared" << std::endl;
    }

    EventManager::~EventManager()
    {
        Clear();
        std::cout << "Event manager destroyed" << std::endl;
    }
    
}
