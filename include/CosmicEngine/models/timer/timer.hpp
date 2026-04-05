#ifndef COSMIC_TIMER_HPP
#define COSMIC_TIMER_HPP

#include <string>

namespace CosmicEngine
{
    class Timer
    {
        private:
            double TargetTime, ElapsedTime;
            bool Loop, Paused, Triggered, Alive, Finished;

        public:
            Timer(double TargetTime, bool Loop, bool Paused);
            ~Timer() = default;

            void update(double deltaTime);

            bool IsTrigger();

            void SetTargetTime(double NewTargetTime);
            double GetTargetTime();
            double GetElapsedTime();
            double GetRemainingTime();
            void EnableLoop();
            void DisableLoop();
            bool HaveLoop();
            bool IsFinished();
            
            void reset();
            void Restart();
            void Pause();
            void Play();
            void Stop();

            bool IsOnPause();

            void Destroy();
            bool IsAlive();

    };
}


#endif // COSMIC_TIMER_HPP