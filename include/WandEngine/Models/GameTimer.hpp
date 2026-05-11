#ifndef GAMETIMER_HPP
#define GAMETIMER_HPP

#include <string>

namespace WandEngine
{
    class GameTimer
    {
        private:
            double TargetTime, ElapsedTime;
            bool Loop, Paused, Triggered, Alive;
            GameTimer* flag;

        public:
            GameTimer(double TargetTime, bool Loop, bool Paused, GameTimer *flag = nullptr);
            ~GameTimer() = default;

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


#endif //GAMETIMER_HPP