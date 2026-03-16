
#ifndef COSMIC_TIMERMANAGER_HPP
#define COSMIC_TIMERMANAGER_HPP

#include "../../models/timer/timer.hpp"
#include <vector>

namespace CosmicEngine
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
        
        void init();
        void shutdown();
        void Add(Timer *NewTimer);
        void update(double deltaTime);
        void Clear();
    };

}

#endif // COSMIC_TIMERMANAGER_HPP
