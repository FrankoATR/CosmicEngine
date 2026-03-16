#ifndef COSMIC_TIMER_HPP
#define COSMIC_TIMER_HPP

#include <string>

namespace CosmicEngine
{
    class Timer
    {
        private:
            double TargetTime, ElapsedTime;
            bool Loop, Paused, Triggered, Alive;
            Timer* flag;

        public:
            Timer(double TargetTime, bool Loop, bool Paused, Timer *flag = nullptr);
            ~Timer() = default;

            void update(double deltaTime);

            bool IsTrigger();

            void SetTargetTime(double NewTargetTime);
            double GetTargetTime();
            double GetElapsedTime();
            void EnableLoop();
            void DisableLoop();
            bool HaveLoop();
            
            void reset();
            void Pause();
            void Play();

            bool IsOnPause();

            void Destroy();
            bool IsAlive();

    };
}


#endif // COSMIC_TIMER_HPP