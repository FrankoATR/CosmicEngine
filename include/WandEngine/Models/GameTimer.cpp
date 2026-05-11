#include "GameTimer.hpp"
#include <iostream>

namespace WandEngine
{
    GameTimer::GameTimer(double TargetTime, bool Loop, bool Paused, GameTimer *flag) :  //Es necesario el flag o quedará mejor en Schedule ? o trabajar solo con almacenar el puntero del Timer/Schedule
        TargetTime(TargetTime), Loop(Loop), Paused(Paused), flag(flag), ElapsedTime(0.0), Alive(true)
    {
    
    }

    void GameTimer::Update(double deltaTime)
    {
        Triggered = false;

        if(!Paused)
        {
            ElapsedTime += deltaTime;

            if (ElapsedTime >= TargetTime)
            {

                Triggered = true;

                ElapsedTime -= TargetTime;

                if(!Loop)
                {
                    Paused = true;
                    ElapsedTime = 0.0;
                    if(flag) flag = nullptr;
                    
                }
            }

        }


    }

    bool GameTimer::IsTrigger()
    {
        return this->Triggered;
    }

    void GameTimer::SetTargetTime(double NewTargetTime)
    {
        this->TargetTime = NewTargetTime;
    }

    double GameTimer::GetTargetTime()
    {
        return this->TargetTime;
    }

    double GameTimer::GetElapsedTime()
    {
        return this->ElapsedTime;
    }

    void GameTimer::EnableLoop()
    {
        this->Loop = true;
    }

    void GameTimer::DisableLoop()
    {
        this->Loop = false;
    }


    bool GameTimer::HaveLoop()
    {
        return this->Loop;
    }

    void GameTimer::Reset()
    {
        this->ElapsedTime = 0.0;
    }

    void GameTimer::Pause()
    {
        this->Paused = true;
    }

    bool GameTimer::IsOnPause()
    {
        return this->Paused;
    }

    void GameTimer::Play()
    {
        this->Paused = false;
    }

    void GameTimer::Destroy()
    {
        this->Alive = false;
    }

    bool GameTimer::IsAlive()
    {
        return this->Alive;
    }
}

