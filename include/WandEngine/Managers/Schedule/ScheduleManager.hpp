
#ifndef SCHEDULEMANAGER_HPP
#define SCHEDULEMANAGER_HPP

#include <vector>

namespace WandEngine
{

    class GameScene;

    class ScheduleManager
    {
    private:
        std::vector<GameScene *> timers;
        
        ScheduleManager();
        ~ScheduleManager();
        ScheduleManager(const ScheduleManager &) = delete;
        ScheduleManager &operator=(const ScheduleManager &) = delete;

    public:
        static ScheduleManager &GetInstance()
        {
            static ScheduleManager instance;
            return instance;
        }

        void Update(double deltaTime);

        void Clear();
    };

    
}

#endif // SCHEDULEMANAGER_HPP
