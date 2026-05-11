#include "TimerManager.hpp"
#include <iostream>

namespace WandEngine
{

    TimerManager::TimerManager(){
        
    }

    void TimerManager::Add(GameTimer *NewTimer){
        Timers.push_back(NewTimer);
    }

    void TimerManager::Update(double deltaTime){
        for (auto it = Timers.begin(); it != Timers.end(); )
        {
            (*it)->Update(deltaTime);

            if (!(*it)->IsAlive())
            {
                GameTimer* tmp = (*it);
                it = Timers.erase(it);
                delete tmp;
            }
            else
            {
                ++it;
            }
        }
    }


    void TimerManager::Clear(){
        while(!Timers.empty())
        {
            delete Timers.back();
            Timers.pop_back();
        }

        #ifndef NDEBUG
            std::cout << "Timer manager cleared" << std::endl;
		#endif
    }

    TimerManager::~TimerManager()
    {
        #ifndef NDEBUG
            std::cout << "Timer manager destroyed" << std::endl;
		#endif
    }

}