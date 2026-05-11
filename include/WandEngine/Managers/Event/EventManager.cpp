#include "EventManager.hpp"

#include <iostream>

namespace WandEngine
{

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
