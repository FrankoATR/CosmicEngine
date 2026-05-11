#ifndef WAND_TIMER_HPP
#define WAND_TIMER_HPP

#include <string>

namespace WandEngine
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

            void Update(double deltaTime);

            bool IsTrigger();

            void SetTargetTime(double NewTargetTime);
            double GetTargetTime();
            double GetElapsedTime();
            void EnableLoop();
            void DisableLoop();
            bool HaveLoop();
            
            void Reset();
            void Pause();
            void Play();

            bool IsOnPause();

            void Destroy();
            bool IsAlive();

    };
}


#endif //WAND_TIMER_HPP