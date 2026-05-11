#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <unordered_map>
#include <vector>
#include <functional>
#include <string>
#include <iostream>

namespace WandEngine
{

    class EventManager
    {
    private:
        EventManager() = default;
        ~EventManager();
        EventManager(const EventManager &) = delete;
        EventManager &operator=(const EventManager &) = delete;

        using EventCallback = std::function<void()>;
        std::unordered_map<std::string, std::vector<EventCallback>> eventCallbacks;

    public:
        static EventManager &GetInstance()
        {
            static EventManager instance;
            return instance;
        }


        void RegisterEvent(const std::string &eventName, EventCallback callback);

        void TriggerEvent(const std::string &eventName);

        void RemoveEvent(const std::string &eventName);

        void Clear();
    };

}

#endif // EVENTMANAGER_HPP
