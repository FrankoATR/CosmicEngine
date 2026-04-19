#include "timer.hpp"

#include "../../utils/log.hpp"

#include <algorithm>

namespace CosmicEngine
{
    Timer::Timer(double TargetTime, bool Loop, bool Paused)
        : TargetTime(std::max(0.0, TargetTime)),
          ElapsedTime(0.0),
          Loop(Loop),
          Paused(Paused),
          Triggered(false),
          Alive(true),
          Finished(false)
    {
		RUNTIME_LIFECYCLE("Timer", "created");
    }

    void Timer::update(double deltaTime)
    {
        Triggered = false;

        if (!Alive || Paused)
        {
            return;
        }

        if (TargetTime <= 0.0)
        {
            Triggered = true;

            if (Loop)
            {
                ElapsedTime = 0.0;
            }
            else
            {
                Finished = true;
                Paused = true;
            }

            return;
        }

        ElapsedTime += std::max(0.0, deltaTime);

        if (Loop)
        {
            if (ElapsedTime >= TargetTime)
            {
                Triggered = true;

                while (ElapsedTime >= TargetTime)
                {
                    ElapsedTime -= TargetTime;
                }
            }

            return;
        }

        if (ElapsedTime >= TargetTime)
        {
            ElapsedTime = TargetTime;
            Triggered = true;
            Finished = true;
            Paused = true;
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

    double Timer::GetRemainingTime()
    {
        return std::max(0.0, TargetTime - ElapsedTime);
    }

    void Timer::EnableLoop()
    {
        this->Loop = true;
        this->Finished = false;
    }

    void Timer::DisableLoop()
    {
        this->Loop = false;
    }


    bool Timer::HaveLoop()
    {
        return this->Loop;
    }

    bool Timer::IsFinished()
    {
        return this->Finished;
    }

    void Timer::reset()
    {
        this->ElapsedTime = 0.0;
        this->Triggered = false;
        this->Finished = false;
    }

    void Timer::Restart()
    {
        reset();
        Play();
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

        if (!Loop && Finished && ElapsedTime >= TargetTime)
        {
            reset();
        }
    }

    void Timer::Stop()
    {
        this->Paused = true;
        this->ElapsedTime = 0.0;
        this->Triggered = false;
        this->Finished = false;
    }

    void Timer::Destroy()
    {
        this->Alive = false;
		RUNTIME_LIFECYCLE("Timer", "destroyed");
    }

    bool Timer::IsAlive()
    {
        return this->Alive;
    }
}

