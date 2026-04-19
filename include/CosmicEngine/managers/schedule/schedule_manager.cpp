/**
 * @file schedule_manager.cpp
 * @brief Implements the runtime scheduler used to trigger deferred and repeated events.
 */

#include "schedule_manager.hpp"

#include "../event/event_manager.hpp"
#include "../../utils/log.hpp"

#include <algorithm>

namespace CosmicEngine
{
    nlohmann::json ScheduledTask::ToJson() const
    {
        return {
            {"id", id},
            {"name", name},
            {"eventName", eventName},
            {"payload", payload},
            {"executeAt", executeAt},
            {"interval", interval},
            {"repeat", repeat},
            {"paused", paused},
            {"alive", alive},
            {"remainingExecutions", remainingExecutions}
        };
    }

    ScheduledTask ScheduledTask::FromJson(const nlohmann::json &json)
    {
        ScheduledTask task;
        task.id = json.value("id", 0ull);
        task.name = json.value("name", std::string());
        task.eventName = json.value("eventName", std::string());
        task.payload = json.value("payload", nlohmann::json::object());
        task.executeAt = json.value("executeAt", 0.0);
        task.interval = json.value("interval", 0.0);
        task.repeat = json.value("repeat", false);
        task.paused = json.value("paused", false);
        task.alive = json.value("alive", true);
        task.remainingExecutions = json.value("remainingExecutions", task.repeat ? -1 : 1);
        return task;
    }

    ScheduleManager::ScheduleManager()
        : currentTime(0.0),
          nextTaskId(1)
    {
        RUNTIME_LIFECYCLE("Schedule manager", "created");
    }

    ScheduleManager::~ScheduleManager()
    {
        shutdown();
        RUNTIME_LIFECYCLE("Schedule manager", "destroyed");
    }

    void ScheduleManager::init()
    {
        currentTime = 0.0;
        nextTaskId = 1;
        tasks.clear();
        RUNTIME_LIFECYCLE("Schedule manager", "initialized");
    }

    void ScheduleManager::shutdown()
    {
        RUNTIME_LIFECYCLE("Schedule manager", "shutdown");
    }

    void ScheduleManager::update(double deltaTime)
    {
        currentTime += std::max(0.0, deltaTime);

        for (ScheduledTask &task : tasks)
        {
            if (!task.alive || task.paused || task.eventName.empty())
            {
                continue;
            }

            // A task can execute multiple times in one update when the engine catches up after a pause.
            while (task.alive && !task.paused && currentTime >= task.executeAt)
            {
                EventManager::GetInstance().TriggerEvent<const nlohmann::json &>(task.eventName, task.payload);

                if (!task.repeat)
                {
                    task.alive = false;
                    break;
                }

                if (task.remainingExecutions > 0)
                {
                    --task.remainingExecutions;
                    if (task.remainingExecutions <= 0)
                    {
                        task.alive = false;
                        break;
                    }
                }

                task.executeAt += std::max(task.interval, 0.0);

                if (task.interval <= 0.0)
                {
                    task.alive = false;
                }
            }
        }

        tasks.erase(
            std::remove_if(tasks.begin(), tasks.end(), [](const ScheduledTask &task)
            {
                return !task.alive;
            }),
            tasks.end());
    }

    std::uint64_t ScheduleManager::ScheduleOnce(double delaySeconds, const std::string &eventName, nlohmann::json payload, const std::string &taskName)
    {
        ScheduledTask task;
        task.id = nextTaskId++;
        task.name = taskName;
        task.eventName = eventName;
        task.payload = std::move(payload);
        task.executeAt = currentTime + std::max(0.0, delaySeconds);
        task.interval = 0.0;
        task.repeat = false;
        task.paused = false;
        task.alive = true;
        task.remainingExecutions = 1;
        tasks.push_back(task);
        return task.id;
    }

    std::uint64_t ScheduleManager::ScheduleInterval(double intervalSeconds, const std::string &eventName, nlohmann::json payload, int repeatCount, const std::string &taskName)
    {
        ScheduledTask task;
        task.id = nextTaskId++;
        task.name = taskName;
        task.eventName = eventName;
        task.payload = std::move(payload);
        task.executeAt = currentTime + std::max(0.0, intervalSeconds);
        task.interval = std::max(0.0, intervalSeconds);
        task.repeat = true;
        task.paused = false;
        task.alive = true;
        task.remainingExecutions = repeatCount;
        tasks.push_back(task);
        return task.id;
    }

    bool ScheduleManager::CancelTask(std::uint64_t taskId)
    {
        for (ScheduledTask &task : tasks)
        {
            if (task.id == taskId)
            {
                task.alive = false;
                return true;
            }
        }

        return false;
    }

    bool ScheduleManager::PauseTask(std::uint64_t taskId)
    {
        for (ScheduledTask &task : tasks)
        {
            if (task.id == taskId)
            {
                task.paused = true;
                return true;
            }
        }

        return false;
    }

    bool ScheduleManager::ResumeTask(std::uint64_t taskId)
    {
        for (ScheduledTask &task : tasks)
        {
            if (task.id == taskId)
            {
                task.paused = false;
                return true;
            }
        }

        return false;
    }

    bool ScheduleManager::HasTask(std::uint64_t taskId) const
    {
        return std::any_of(tasks.begin(), tasks.end(), [taskId](const ScheduledTask &task)
        {
            return task.id == taskId && task.alive;
        });
    }

    double ScheduleManager::GetCurrentTime() const
    {
        return currentTime;
    }

    nlohmann::json ScheduleManager::ExportTasks() const
    {
        nlohmann::json exported = nlohmann::json::array();
        for (const ScheduledTask &task : tasks)
        {
            if (task.alive)
            {
                exported.push_back(task.ToJson());
            }
        }

        return exported;
    }

    void ScheduleManager::ImportTasks(const nlohmann::json &tasksJson, bool append)
    {
        if (!append)
        {
            tasks.clear();
        }

        if (!tasksJson.is_array())
        {
            return;
        }

        for (const nlohmann::json &entry : tasksJson)
        {
            ScheduledTask task = ScheduledTask::FromJson(entry);
            nextTaskId = std::max(nextTaskId, task.id + 1);
            tasks.push_back(std::move(task));
        }
    }

    void ScheduleManager::Clear()
    {
        tasks.clear();
        currentTime = 0.0;
        nextTaskId = 1;
        RUNTIME_LIFECYCLE("Schedule manager", "cleared");
    }
}