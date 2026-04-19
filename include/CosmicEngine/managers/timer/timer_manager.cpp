/**
 * @file timer_manager.cpp
 * @brief Implements the runtime timer manager used by the engine.
 */

#include "timer_manager.hpp"
#include <iostream>

namespace CosmicEngine
{

    TimerManager &TimerManager::GetInstance()
    {
        static TimerManager instance;
        return instance;
    }

    TimerManager::TimerManager()
    {
        std::cout << "Timer manager created" << std::endl;
    }


    TimerManager::~TimerManager()
    {
        shutdown();
        std::cout << "Timer manager destroyed" << std::endl;
    }

    void TimerManager::init()
    {
        std::cout << "Timer manager initialized" << std::endl;
    }

    
    void TimerManager::shutdown()
    {
        
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

        std::cout << "Timer manager cleared" << std::endl;
    }



}