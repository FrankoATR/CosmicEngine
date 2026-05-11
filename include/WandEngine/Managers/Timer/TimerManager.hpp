
#ifndef TIMERMANAGER_HPP
#define TIMERMANAGER_HPP

#include "../../Models/GameTimer.hpp"
#include <vector>

namespace WandEngine
{

    class GameScene;

    class TimerManager
    {
    private:
        std::vector<GameTimer *> Timers;

        TimerManager();
        ~TimerManager();
        TimerManager(const TimerManager &) = delete;
        TimerManager &operator=(const TimerManager &) = delete;

    public:
        static TimerManager &GetInstance()
        {
            static TimerManager instance;
            return instance;
        }

        void Add(GameTimer *NewTimer);
        void Update(double deltaTime);
        void Clear();
    };

}

#endif // TIMERMANAGER_HPP
