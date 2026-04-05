#include "event_manager.hpp"

#include <iostream>

namespace CosmicEngine
{
    EventManager &EventManager::GetInstance()
    {
        static EventManager instance;
        return instance;
    }

    void EventManager::init()
    {
        std::cout << "Event manager created" << std::endl;
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
        
        #ifndef NDEBUG
            std::cout << "Event manager cleared" << std::endl;
		#endif
    }

    EventManager::~EventManager()
    {
        #ifndef NDEBUG
            std::cout << "Event manager destroyed" << std::endl;
		#endif
    }

}
