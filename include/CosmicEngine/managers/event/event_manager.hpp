#ifndef COSMIC_EVENTMANAGER_HPP
#define COSMIC_EVENTMANAGER_HPP

#include "../../interfaces/event.hpp"
#include "../../interfaces/ievent.hpp"
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <string>
#include <iostream>

namespace CosmicEngine
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
        using ListenerId = std::uint64_t;

        static EventManager &GetInstance();
        
        void init();

        bool HasEvent(const std::string &eventName) const;

        template <typename... Args>
        ListenerId RegisterEventListener(const std::string &eventName, std::function<void(Args...)> callback)
        {
            IEvent* eventBase = nullptr;
            auto it = eventCallbacks.find(eventName);
            if (it == eventCallbacks.end())
            {
                auto event = new Event<Args...>();
                ListenerId listenerId = event->AddListener(std::move(callback));
                eventCallbacks[eventName] = event;
                return listenerId;
            }

            eventBase = it->second;
            auto event = dynamic_cast<Event<Args...>*>(eventBase);
            if (event)
            {
                return event->AddListener(std::move(callback));
            }

            throw std::runtime_error("Tipos de parámetros del evento no coinciden al registrar.");
        }

        template <typename... Args>
        void RegisterEvent(const std::string &eventName, std::function<void(Args...)> callback)
        {
            RegisterEventListener<Args...>(eventName, std::move(callback));
        }

        template <typename... Args>
        bool UnregisterListener(const std::string &eventName, ListenerId listenerId)
        {
            auto it = eventCallbacks.find(eventName);
            if (it == eventCallbacks.end())
            {
                return false;
            }

            IEvent *eventBase = it->second;
            auto event = dynamic_cast<Event<Args...>*>(eventBase);
            if (!event)
            {
                throw std::runtime_error("Tipos de parámetros del evento no coinciden al desregistrar.");
            }

            return event->RemoveListener(listenerId);
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

#endif // COSMIC_EVENTMANAGER_HPP
