
#ifndef COSMIC_SCHEDULEMANAGER_HPP
#define COSMIC_SCHEDULEMANAGER_HPP

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace CosmicEngine
{
    struct ScheduledTask
    {
        std::uint64_t id = 0;
        std::string name;
        std::string eventName;
        nlohmann::json payload = nlohmann::json::object();
        double executeAt = 0.0;
        double interval = 0.0;
        bool repeat = false;
        bool paused = false;
        bool alive = true;
        int remainingExecutions = 1;

        nlohmann::json ToJson() const;
        static ScheduledTask FromJson(const nlohmann::json &json);
    };

    class ScheduleManager
    {
    private:
        std::vector<ScheduledTask> tasks;
        double currentTime;
        std::uint64_t nextTaskId;
        
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

        void init();
        void shutdown();

        void update(double deltaTime);

        std::uint64_t ScheduleOnce(
            double delaySeconds,
            const std::string &eventName,
            nlohmann::json payload = nlohmann::json::object(),
            const std::string &taskName = "");

        std::uint64_t ScheduleInterval(
            double intervalSeconds,
            const std::string &eventName,
            nlohmann::json payload = nlohmann::json::object(),
            int repeatCount = -1,
            const std::string &taskName = "");

        bool CancelTask(std::uint64_t taskId);
        bool PauseTask(std::uint64_t taskId);
        bool ResumeTask(std::uint64_t taskId);
        bool HasTask(std::uint64_t taskId) const;

        double GetCurrentTime() const;
        nlohmann::json ExportTasks() const;
        void ImportTasks(const nlohmann::json &tasksJson, bool append = false);

        void Clear();
    };

    
}

#endif // COSMIC_SCHEDULEMANAGER_HPP
