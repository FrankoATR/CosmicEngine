#include "TimerManager.hpp"
#include <iostream>

namespace WandEngine
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
        Shutdown();
        std::cout << "Timer manager destroyed" << std::endl;
    }

    void TimerManager::Init()
    {
        std::cout << "Timer manager initialized" << std::endl;
    }

    
    void TimerManager::Shutdown()
    {
        
    }
    

    void TimerManager::Add(GameTimer *NewTimer)
    {
        Timers.push_back(NewTimer);
    }

    void TimerManager::Update(double deltaTime)
    {
        for (auto it = Timers.begin(); it != Timers.end();)
        {
            (*it)->Update(deltaTime);

            if (!(*it)->IsAlive())
            {
                GameTimer *tmp = (*it);
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