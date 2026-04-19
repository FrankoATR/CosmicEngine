/**
 * @file event_manager.cpp
 * @brief Implements the generic event bus used by the engine.
 */

#include "event_manager.hpp"

namespace CosmicEngine
{
    EventManager &EventManager::GetInstance()
    {
        static EventManager instance;
        return instance;
    }

    void EventManager::init()
    {
        RUNTIME_LIFECYCLE("Event manager", "initialized");
    }

    bool EventManager::HasEvent(const std::string &eventName) const
    {
        return eventCallbacks.find(eventName) != eventCallbacks.end();
    }

    void EventManager::RemoveEvent(const std::string &eventName)
    {
            auto it = eventCallbacks.find(eventName);
            if (it != eventCallbacks.end())
            {
                delete it->second;
                eventCallbacks.erase(it);
            }
    }

    void EventManager::Clear()
    {
        for (auto& pair : eventCallbacks)
        {
            delete pair.second;
        }
        eventCallbacks.clear();

                RUNTIME_LIFECYCLE("Event manager", "cleared");
    }

    EventManager::~EventManager()
    {
                RUNTIME_LIFECYCLE("Event manager", "destroyed");
    }

}
