#include "GameTimer.hpp"

namespace WandEngine
{
    GameTimer::GameTimer(double WaitTime, bool Loop, bool Paused) :
        WaitTime(WaitTime), Loop(Loop), Paused(Paused), Ended(false)
    {
        
    }

    void GameTimer::Update(double CurrentTime)
    {
        if ( !Paused && CurrentTime - LastTimeTrigger >= WaitTime)
        {
            LastTimeTrigger = CurrentTime;

            Triggered = true;

            if(!Loop)
            {
                End();
            }
        }
        else
        {
            Triggered = false;
        }
    }

    bool GameTimer::IsTrigger()
    {
        return this->Triggered;
    }

    void GameTimer::SetWaitTime(double NewWaitTime)
    {
        this->WaitTime = NewWaitTime;
    }

    double GameTimer::GetWaitTime()
    {
        return this->WaitTime;
    }

    
    /*
    void GameTimer::SetLoop(bool HaveLoop)
    {
        this->Loop = HaveLoop;
    }
    */

    bool GameTimer::HaveLoop()
    {
        return this->Loop;
    }

    void GameTimer::Reset()
    {
        this->LastTimeTrigger = 0.0;
    }

    void GameTimer::Pause()
    {
        this->Paused = true;
    }

    void GameTimer::Play()
    {
        this->Paused = false;
    }

    void GameTimer::End()
    {
        this->Ended = true;
    }

    bool GameTimer::IsEnded()
    {
        return this->Ended;
    }
}

