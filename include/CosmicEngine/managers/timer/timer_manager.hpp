
#ifndef COSMIC_TIMERMANAGER_HPP
#define COSMIC_TIMERMANAGER_HPP

/**
 * @file timer_manager.hpp
 * @brief Declares the timer manager used by the engine runtime.
 */

#include "../../models/timer/timer.hpp"
#include <vector>

namespace CosmicEngine
{

    class GameScene;

    /**
     * @brief Owns runtime timers and updates them every frame.
     *
     * TimerManager is the singleton that ticks all registered Timer instances.
     * Create a Timer, add it with TMR_MN.Add(), and poll timer->IsTrigger()
     * each frame from your entity code.
     *
     * @par Example
     * @code
     * auto* timer = new CosmicEngine::Timer(2.0, false, false);
     * CosmicEngine::TimerManager::GetInstance().Add(timer);
     * @endcode
     */
    class TimerManager
    {
    private:
        std::vector<Timer *> Timers;

        TimerManager();
        ~TimerManager();
        TimerManager(const TimerManager &) = delete;
        TimerManager &operator=(const TimerManager &) = delete;

    public:
        /** @brief Returns the singleton instance of the timer manager. */
        static TimerManager &GetInstance();
        
        /** @brief Initializes the timer manager state. */
        void init();
        /** @brief Shuts the timer manager down. */
        void shutdown();
        /** @brief Adds a timer to the runtime. */
        void Add(Timer *NewTimer);
        /** @brief Updates all managed timers. */
        void update(double deltaTime);
        /** @brief Deletes and removes every managed timer. */
        void Clear();
    };

}

#endif // COSMIC_TIMERMANAGER_HPP
