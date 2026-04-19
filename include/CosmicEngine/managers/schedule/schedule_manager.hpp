
#ifndef COSMIC_SCHEDULEMANAGER_HPP
#define COSMIC_SCHEDULEMANAGER_HPP

/**
 * @file schedule_manager.hpp
 * @brief Declares the scheduler used to fire deferred or repeated runtime events.
 */

#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace CosmicEngine
{
    /**
     * @brief Represents a scheduled event task.
     */
    struct ScheduledTask
    {
        /** @brief Unique task identifier. */
        std::uint64_t id = 0;
        /** @brief Optional human-readable task name. */
        std::string name;
        /** @brief Event name to trigger when the task executes. */
        std::string eventName;
        /** @brief JSON payload passed when the task executes. */
        nlohmann::json payload = nlohmann::json::object();
        /** @brief Absolute execution time in scheduler seconds. */
        double executeAt = 0.0;
        /** @brief Repeat interval in seconds when the task is periodic. */
        double interval = 0.0;
        /** @brief True when the task should repeat. */
        bool repeat = false;
        /** @brief True when the task is currently paused. */
        bool paused = false;
        /** @brief True when the task is still alive in the runtime. */
        bool alive = true;
        /** @brief Remaining execution count, or -1 for infinite repetition. */
        int remainingExecutions = 1;

        /** @brief Serializes the task to JSON. */
        nlohmann::json ToJson() const;
        /** @brief Deserializes a task from JSON. */
        static ScheduledTask FromJson(const nlohmann::json &json);
    };

    /**
     * @brief Manages deferred and repeated event tasks.
     *
     * ScheduleManager fires named events after a time delay or at a fixed interval.
     * It works in tandem with the EventManager: each scheduled task triggers the
     * named event with a JSON payload when due.  Use ScheduleOnce() for one-shot
     * timers and ScheduleInterval() for repeating tasks.
     *
     * @par Example — spawning objects every 2 seconds
     * @code
     * nlohmann::json payload = {
     *     {"randomPosition", true},
     *     {"size", {64.0f, 64.0f}},
     *     {"health", 100}
     * };
     *
     * // -1 = infinite repetitions
     * auto taskId = SCH_MN.ScheduleInterval(
     *     2.0,                                      // interval seconds
     *     "spawn_entity",                            // event name
     *     payload,                                   // JSON payload
     *     -1,                                        // repeat count
     *     "spawn_random_object_every_2_seconds");    // human-readable name
     *
     * // Cancel later:
     * SCH_MN.CancelTask(taskId);
     * @endcode
     */
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
        /** @brief Returns the singleton instance of the schedule manager. */
        static ScheduleManager &GetInstance()
        {
            static ScheduleManager instance;
            return instance;
        }

        /** @brief Initializes the schedule manager state. */
        void init();
        /** @brief Shuts the schedule manager down. */
        void shutdown();

        /** @brief Advances scheduler time and executes due tasks. */
        void update(double deltaTime);

        /** @brief Schedules a one-shot event to run after a delay. */
        std::uint64_t ScheduleOnce(
            double delaySeconds,
            const std::string &eventName,
            nlohmann::json payload = nlohmann::json::object(),
            const std::string &taskName = "");

        /** @brief Schedules a repeating event with an optional execution count. */
        std::uint64_t ScheduleInterval(
            double intervalSeconds,
            const std::string &eventName,
            nlohmann::json payload = nlohmann::json::object(),
            int repeatCount = -1,
            const std::string &taskName = "");

        /** @brief Cancels a scheduled task by identifier. */
        bool CancelTask(std::uint64_t taskId);
        /** @brief Pauses a scheduled task by identifier. */
        bool PauseTask(std::uint64_t taskId);
        /** @brief Resumes a scheduled task by identifier. */
        bool ResumeTask(std::uint64_t taskId);
        /** @brief Returns whether a task with the provided identifier exists. */
        bool HasTask(std::uint64_t taskId) const;

        /** @brief Returns the current scheduler time in seconds. */
        double GetCurrentTime() const;
        /** @brief Serializes all scheduled tasks to JSON. */
        nlohmann::json ExportTasks() const;
        /** @brief Imports scheduled tasks from JSON, optionally appending to the current task list. */
        void ImportTasks(const nlohmann::json &tasksJson, bool append = false);

        /** @brief Clears every scheduled task. */
        void Clear();
    };

    
}

#endif // COSMIC_SCHEDULEMANAGER_HPP
