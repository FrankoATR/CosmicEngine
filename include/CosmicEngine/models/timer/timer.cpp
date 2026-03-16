#include "timer.hpp"
#include <iostream>

namespace CosmicEngine
{
    Timer::Timer(double TargetTime, bool Loop, bool Paused, Timer *flag) :  //Es necesario el flag o quedará mejor en Schedule ? o trabajar solo con almacenar el puntero del Timer/Schedule
        TargetTime(TargetTime), Loop(Loop), Paused(Paused), flag(flag), ElapsedTime(0.0), Alive(true)
    {
    
    }

    void Timer::update(double deltaTime)
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

    bool Timer::IsTrigger()
    {
        return this->Triggered;
    }

    void Timer::SetTargetTime(double NewTargetTime)
    {
        this->TargetTime = NewTargetTime;
    }

    double Timer::GetTargetTime()
    {
        return this->TargetTime;
    }

    double Timer::GetElapsedTime()
    {
        return this->ElapsedTime;
    }

    void Timer::EnableLoop()
    {
        this->Loop = true;
    }

    void Timer::DisableLoop()
    {
        this->Loop = false;
    }


    bool Timer::HaveLoop()
    {
        return this->Loop;
    }

    void Timer::reset()
    {
        this->ElapsedTime = 0.0;
    }

    void Timer::Pause()
    {
        this->Paused = true;
    }

    bool Timer::IsOnPause()
    {
        return this->Paused;
    }

    void Timer::Play()
    {
        this->Paused = false;
    }

    void Timer::Destroy()
    {
        this->Alive = false;
    }

    bool Timer::IsAlive()
    {
        return this->Alive;
    }
}

