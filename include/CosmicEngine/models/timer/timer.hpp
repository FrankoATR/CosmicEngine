#ifndef COSMIC_TIMER_HPP
#define COSMIC_TIMER_HPP

/**
 * @file timer.hpp
 * @brief Declares the timer utility used by the engine runtime.
 */

#include <string>

namespace CosmicEngine
{
    /**
     * @brief Represents a configurable runtime timer with pause and looping support.
     *
     * Timers are updated each frame by the TimerManager.  Create one, register it
     * with TMR_MN.Add(), and poll IsTrigger() in your update loop to react when the
     * target duration has elapsed.
     *
     * @par Example — creating and checking a timer
     * @code
     * // Create a 2-second non-looping timer that starts running immediately
     * directionChangeTimer = new CosmicEngine::Timer(2.0, false, false);
     * CosmicEngine::TimerManager::GetInstance().Add(directionChangeTimer);
     *
     * // Inside your entity update callback:
     * void MyEntity::update(float deltaTime)
     * {
     *     if (directionChangeTimer && directionChangeTimer->IsTrigger())
     *     {
     *         PickRandomDirection();
     *         directionChangeTimer->SetTargetTime(1.5);
     *         directionChangeTimer->reset();
     *         directionChangeTimer->Play();
     *     }
     * }
     * @endcode
     *
     * @par Example — destroying a timer
     * @code
     * if (directionChangeTimer)
     * {
     *     directionChangeTimer->Destroy();
     *     directionChangeTimer = nullptr;
     * }
     * @endcode
     */
    class Timer
    {
        private:
            double TargetTime, ElapsedTime;
            bool Loop, Paused, Triggered, Alive, Finished;

        public:
            /**
             * @brief Creates a timer.
             * @param TargetTime Trigger duration in seconds.
             * @param Loop True to restart automatically after triggering.
             * @param Paused True to start in the paused state.
             */
            Timer(double TargetTime, bool Loop, bool Paused);
            ~Timer() = default;

            /**
             * @brief Advances the timer state.
             * @param deltaTime Time step in seconds.
             */
            void update(double deltaTime);

            /**
             * @brief Returns whether the timer has triggered.
             * @return True when the timer has reached its target time.
             */
            bool IsTrigger();

            /** @brief Sets the target time in seconds. */
            void SetTargetTime(double NewTargetTime);
            /** @brief Returns the target time in seconds. */
            double GetTargetTime();
            /** @brief Returns the elapsed time in seconds. */
            double GetElapsedTime();
            /** @brief Returns the remaining time in seconds. */
            double GetRemainingTime();
            /** @brief Enables looping mode. */
            void EnableLoop();
            /** @brief Disables looping mode. */
            void DisableLoop();
            /** @brief Returns whether looping mode is enabled. */
            bool HaveLoop();
            /** @brief Returns whether the timer finished its current run. */
            bool IsFinished();
            
            /** @brief Resets the timer state without changing its configuration. */
            void reset();
            /** @brief Restarts the timer from the beginning and resumes it. */
            void Restart();
            /** @brief Pauses the timer. */
            void Pause();
            /** @brief Resumes the timer. */
            void Play();
            /** @brief Stops the timer and marks it as finished. */
            void Stop();

            /** @brief Returns whether the timer is currently paused. */
            bool IsOnPause();

            /** @brief Marks the timer for removal from the runtime. */
            void Destroy();
            /** @brief Returns whether the timer is still alive in the runtime. */
            bool IsAlive();

    };
}


#endif // COSMIC_TIMER_HPP