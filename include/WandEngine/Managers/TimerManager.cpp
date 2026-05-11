#include "TimerManager.hpp"

namespace WandEngine
{

    TimerManager::TimerManager(){
        
    }

    void TimerManager::Add(GameTimer *NewTimer){
        Timers.push_back(NewTimer);
    }

    void TimerManager::Update(double CurrentTime){
        for (auto it = Timers.begin(); it != Timers.end(); )
        {
            (*it)->Update(CurrentTime);

            if ((*it)->IsEnded())
            {
                it = Timers.erase(it);
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