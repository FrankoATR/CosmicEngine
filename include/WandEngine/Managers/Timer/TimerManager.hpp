
#ifndef TIMERMANAGER_HPP
#define TIMERMANAGER_HPP

#include "../../Models/Timer/Timer.hpp"
#include <vector>

namespace WandEngine
{

    class GameScene;

    class TimerManager
    {
    private:
        std::vector<Timer *> Timers;

        TimerManager();
        ~TimerManager();
        TimerManager(const TimerManager &) = delete;
        TimerManager &operator=(const TimerManager &) = delete;

    public:
        static TimerManager &GetInstance();
        
        void Init();
        void Shutdown();
        void Add(Timer *NewTimer);
        void Update(double deltaTime);
        void Clear();
    };

}

#endif // TIMERMANAGER_HPP
