#ifndef COSMIC_EVENT_HPP
#define COSMIC_EVENT_HPP

/**
 * @file event.hpp
 * @brief Declares the typed event container used by the engine event bus.
 */

#include "ievent.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>
#include <functional>

/**
 * @brief Stores listeners for a typed event signature.
 *
 * Event\<Args...\> is a typed container that holds a list of callbacks with the
 * same signature.  The EventManager owns Event instances internally, one per
 * named event.  You typically interact with events through the EventManager API
 * rather than using this class directly.
 *
 * @tparam Args Event argument types forwarded to every listener callback.
 *
 * @par Example — internal usage pattern (via EventManager)
 * @code
 * // EventManager creates an Event<const nlohmann::json&> behind the scenes:
 * EVT_MN.RegisterEventListener<const nlohmann::json&>(
 *     "spawn_entity",
 *     std::function<void(const nlohmann::json&)>([](const nlohmann::json& p) {
 *         std::cout << p.dump() << std::endl;
 *     }));
 * @endcode
 */
template<typename... Args>
class Event : public IEvent {
public:
    /** @brief Callable signature accepted by listeners of this event. */
    using CallbackType = std::function<void(Args...)>;
    /** @brief Opaque identifier returned by AddListener and used to remove a listener. */
    using ListenerId = std::uint64_t;

    /**
     * @brief Registers a new listener.
     * @param callback Callback to store.
     * @return Listener identifier.
     */
    ListenerId AddListener(CallbackType callback) {
        ListenerId id = nextListenerId++;
        callbacks.push_back({id, std::move(callback)});
        return id;
    }

    /**
     * @brief Removes a listener by identifier.
     * @param id Listener identifier.
     * @return True when a listener was removed.
     */
    bool RemoveListener(ListenerId id) {
        auto oldSize = callbacks.size();
        callbacks.erase(
            std::remove_if(callbacks.begin(), callbacks.end(), [id](const ListenerEntry &entry) {
                return entry.id == id;
            }),
            callbacks.end());
        return callbacks.size() != oldSize;
    }

    /**
     * @brief Invokes all registered listeners.
     * @param args Event arguments.
     */
    void Invoke(Args... args) {
        for (auto& callback : callbacks) {
            callback.fn(args...);
        }
    }

private:
    /** @brief Internal record pairing a listener identifier with its callback. */
    struct ListenerEntry {
        ListenerId id;     ///< Unique listener identifier within this event.
        CallbackType fn;   ///< Stored callable invoked on @ref Invoke.
    };

    std::vector<ListenerEntry> callbacks;   ///< Registered listeners in insertion order.
    ListenerId nextListenerId = 1;          ///< Counter used to assign unique listener IDs.
};

#endif // COSMIC_EVENT_HPP
