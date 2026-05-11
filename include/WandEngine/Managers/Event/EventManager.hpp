#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include "../../Interfaces/Event.hpp"
#include "../../Interfaces/IEvent.hpp"
#include <unordered_map>
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

        std::unordered_map<std::string, IEvent*> eventCallbacks;

    public:
        static EventManager &GetInstance();
        
        void Init();

        template <typename... Args>
        void RegisterEvent(const std::string &eventName, std::function<void(Args...)> callback)
        {
            IEvent* eventBase = nullptr;
            auto it = eventCallbacks.find(eventName);
            if (it == eventCallbacks.end())
            {
                auto event = new Event<Args...>();
                event->AddListener(callback);
                eventCallbacks[eventName] = event;
            }
            else
            {
                eventBase = it->second;
                auto event = dynamic_cast<Event<Args...>*>(eventBase);
                if (event)
                {
                    event->AddListener(callback);
                }
                else
                {
                    throw std::runtime_error("Tipos de parámetros del evento no coinciden al registrar.");
                }
            }
        }

        template <typename... Args>
        void TriggerEvent(const std::string &eventName, Args... args)
        {
            auto it = eventCallbacks.find(eventName);
            if (it != eventCallbacks.end())
            {
                IEvent* eventBase = it->second;
                auto event = dynamic_cast<Event<Args...>*>(eventBase);
                if (event)
                {
                    event->Invoke(args...);
                }
                else
                {
                    throw std::runtime_error("Tipos de parámetros del evento no coinciden al disparar.");
                }
            }
            else
            {
                std::cerr << "Evento no encontrado: " << eventName << std::endl;
            }
        }


        void RemoveEvent(const std::string &eventName);

        void Clear();
    };

}

#endif // EVENTMANAGER_HPP
