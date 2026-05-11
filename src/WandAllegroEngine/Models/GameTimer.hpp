#ifndef GAMETIMER_HPP
#define GAMETIMER_HPP

#include <string>

namespace WandEngine
{
    class GameTimer
    {
        private:
            double WaitTime, LastTimeTrigger;
            bool Init, Loop, Paused, Triggered, Ended;

        public:
            GameTimer(double WaitTime, bool Loop, bool Paused);
            ~GameTimer() = default;

            void Update(double CurrentTime);

            bool IsTrigger();

            void SetWaitTime(double NewWaitTime);
            double GetWaitTime();
            
            //void SetLoop(bool HaveLoop);
            bool HaveLoop();

            void Reset();
            void Pause();
            void Play();
            void End();

            bool IsEnded();

    };
}


#endif //GAMETIMER_HPP