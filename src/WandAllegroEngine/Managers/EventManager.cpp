#include "EventManager.hpp"
#include "../Interfaces/GameEvent.hpp"

namespace WandEngine
{

    void EventManager::RegisterEvent(const std::string &eventName, EventCallback callback)
    {
        eventCallbacks[eventName].push_back(callback);
    }

    void EventManager::TriggerEvent(const std::string &eventName)
    {
        if (eventCallbacks.find(eventName) != eventCallbacks.end())
        {
            for (const auto &callback : eventCallbacks[eventName])
            {
                callback();
            }
        }
        else
        {
            std::cout << "Event not found: " << eventName << std::endl;
        }
    }

    void EventManager::RemoveEvent(const std::string &eventName)
    {
        eventCallbacks.erase(eventName);
    }

    void EventManager::Clear()
    {
        eventCallbacks.clear();
        std::cout << "Event manager cleared" << std::endl;
    }

    EventManager::~EventManager()
    {
        Clear();
        std::cout << "Event manager destroyed" << std::endl;
    }

}
