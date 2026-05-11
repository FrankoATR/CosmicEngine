#ifndef GAMETIMER_HPP
#define GAMETIMER_HPP

#include <string>

namespace WandEngine
{
    class GameTimer
    {
        private:
            double WaitTime, LastTimeTrigger;
            bool Init, ResetFlag, Loop, Paused, Triggered, Ended, *flag;

        public:
            GameTimer(double WaitTime, bool Loop, bool Paused, bool *flag);
            GameTimer(double WaitTime, bool Loop, bool Paused);
            ~GameTimer() = default;

            void Update(double CurrentTime);

            bool IsTrigger();

            void SetWaitTime(double NewWaitTime);
            double GetWaitTime();
            double GetElapsedTime();
            //void SetLoop(bool HaveLoop);
            bool HaveLoop();
            
            void Reset();
            void Pause();
            void Play();
            void End();

            bool IsPause();
            bool IsEnded();

    };
}


#endif //GAMETIMER_HPP