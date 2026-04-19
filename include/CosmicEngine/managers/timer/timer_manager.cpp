/**
 * @file timer_manager.cpp
 * @brief Implements the runtime timer manager used by the engine.
 */

#include "timer_manager.hpp"
#include "../../utils/log.hpp"

namespace CosmicEngine
{

    TimerManager &TimerManager::GetInstance()
    {
        static TimerManager instance;
        return instance;
    }

    TimerManager::TimerManager()
    {
        RUNTIME_LIFECYCLE("Timer manager", "created");
    }


    TimerManager::~TimerManager()
    {
        shutdown();
        RUNTIME_LIFECYCLE("Timer manager", "destroyed");
    }

    void TimerManager::init()
    {
        RUNTIME_LIFECYCLE("Timer manager", "initialized");
    }

    
    void TimerManager::shutdown()
    {
        RUNTIME_LIFECYCLE("Timer manager", "shutdown");
    }
    

    void TimerManager::Add(Timer *NewTimer)
    {
        Timers.push_back(NewTimer);
    }

    void TimerManager::update(double deltaTime)
    {
        for (auto it = Timers.begin(); it != Timers.end();)
        {
            (*it)->update(deltaTime);

            if (!(*it)->IsAlive())
            {
                Timer *tmp = (*it);
                it = Timers.erase(it);
                delete tmp;
            }
            else
            {
                ++it;
            }
        }
    }

    void TimerManager::Clear()
    {
        while (!Timers.empty())
        {
            delete Timers.back();
            Timers.pop_back();
        }

        RUNTIME_LIFECYCLE("Timer manager", "cleared");
    }



}