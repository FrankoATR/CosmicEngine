
#ifndef COSMIC_SCHEDULEMANAGER_HPP
#define COSMIC_SCHEDULEMANAGER_HPP

#include <vector>

namespace CosmicEngine
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

        void update(double deltaTime);

        void Clear();
    };

    
}

#endif // COSMIC_SCHEDULEMANAGER_HPP
