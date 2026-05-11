#include "GameTimer.hpp"
#include <iostream>
namespace WandEngine
{
    GameTimer::GameTimer(double WaitTime, bool Loop, bool Paused, bool *flag) :  //Es necesario el flag o quedará mejor en Schedule ? o trabajar solo con almacenar el puntero del Timer/Schedule
        WaitTime(WaitTime), Loop(Loop), Paused(Paused), flag(flag), Ended(false), Init(true), ResetFlag(false)
    {
        
    }

    GameTimer::GameTimer(double WaitTime, bool Loop, bool Paused) :
        WaitTime(WaitTime), Loop(Loop), Paused(Paused), flag(nullptr), Ended(false), Init(true), ResetFlag(false)
    {
        
    }

    void GameTimer::Update(double CurrentTime)
    {
        if(ResetFlag)
        {
            LastTimeTrigger = CurrentTime;
            ResetFlag = false;
        }

        if(!Paused && Init)
        {
            LastTimeTrigger = CurrentTime;
            Init = false;
        }

        if ( !Paused && CurrentTime - LastTimeTrigger >= WaitTime)
        {
            LastTimeTrigger = CurrentTime;

            Triggered = true;
            if(flag) *flag = true;

            if(!Loop)
            {
                End();
            }
        }
        else
        {
            Triggered = false;
            //if(flag) *flag = false;

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

    double GameTimer::GetElapsedTime()
    {
        return this->LastTimeTrigger;
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
        this->ResetFlag = true;
    }

    void GameTimer::Pause()
    {
        this->Paused = true;
    }

    bool GameTimer::IsPause()
    {
        return this->Paused;
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

