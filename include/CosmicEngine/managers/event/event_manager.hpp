#ifndef COSMIC_EVENTMANAGER_HPP
#define COSMIC_EVENTMANAGER_HPP

/**
 * @file event_manager.hpp
 * @brief Declares the generic event bus used by the engine.
 */

#include "../../interfaces/event.hpp"
#include "../../interfaces/ievent.hpp"
#include "../../utils/log.hpp"
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <string>

namespace CosmicEngine
{

    /**
     * @brief Stores named events and dispatches typed callbacks to registered listeners.
     *
     * EventManager is a generic event bus.  Events are identified by a unique
     * string name and can carry arbitrary typed arguments.  Listeners are registered
     * with RegisterEventListener() which returns a ListenerId for later removal.
     * The ScheduleManager fires events by name, so the two systems work together.
     *
    * @par Example - register a listener and trigger it
     * @code
     * // Register a listener that receives a JSON payload:
     * auto listenerId = EVT_MN.RegisterEventListener<const nlohmann::json&>(
     *     "spawn_entity",
     *     std::function<void(const nlohmann::json&)>([this](const nlohmann::json& payload) {
     *         SpawnFromPayload(payload);
     *     }));
     *
     * // Trigger the event manually:
     * nlohmann::json data = {{"type", "enemy"}, {"x", 100}, {"y", 200}};
     * EVT_MN.TriggerEvent<const nlohmann::json&>("spawn_entity", data);
     *
     * // Unregister when done:
     * EVT_MN.UnregisterListener<const nlohmann::json&>("spawn_entity", listenerId);
     * @endcode
     */
    class EventManager
    {
    private:
        EventManager() = default;
        ~EventManager();
        EventManager(const EventManager &) = delete;
        EventManager &operator=(const EventManager &) = delete;

        std::unordered_map<std::string, IEvent*> eventCallbacks;

    public:
        /** @brief Identifier type used for registered event listeners. */
        using ListenerId = std::uint64_t;

        /** @brief Returns the singleton instance of the event manager. */
        static EventManager &GetInstance();
        
        /** @brief Initializes the event manager state. */
        void init();

        /** @brief Returns whether an event with the provided name exists. */
        bool HasEvent(const std::string &eventName) const;

        /**
         * @brief Registers a typed listener for an event and returns its listener identifier.
         * @tparam Args Event argument types.
         * @param eventName Event name.
         * @param callback Callback invoked when the event is triggered.
         * @return Listener identifier used for unregistration.
         */
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

            throw std::runtime_error("Event parameter types do not match during registration.");
        }

        /**
         * @brief Registers a typed listener for an event.
         * @tparam Args Event argument types.
         * @param eventName Event name.
         * @param callback Callback invoked when the event is triggered.
         */
        template <typename... Args>
        void RegisterEvent(const std::string &eventName, std::function<void(Args...)> callback)
        {
            RegisterEventListener<Args...>(eventName, std::move(callback));
        }

        /**
         * @brief Unregisters a listener from an event.
         * @tparam Args Event argument types.
         * @param eventName Event name.
         * @param listenerId Listener identifier to remove.
         * @return True when the listener was removed.
         */
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
                throw std::runtime_error("Event parameter types do not match during unregistration.");
            }

            return event->RemoveListener(listenerId);
        }

        /**
         * @brief Triggers a named event and forwards the provided arguments to its listeners.
         * @tparam Args Event argument types.
         * @param eventName Event name.
         * @param args Event arguments.
         */
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
                    throw std::runtime_error("Event parameter types do not match during dispatch.");
                }
            }
            else
            {
				RUNTIME_WARNING("Event not found: " << eventName);
            }
        }


        /** @brief Removes an event and all its listeners. */
        void RemoveEvent(const std::string &eventName);

        /** @brief Clears every registered event and listener. */
        void Clear();
    };

}

#endif // COSMIC_EVENTMANAGER_HPP
