#ifndef COSMIC_EVENT_HPP
#define COSMIC_EVENT_HPP

#include "ievent.hpp"
#include <algorithm>
#include <cstdint>
#include <vector>
#include <functional>

template<typename... Args>
class Event : public IEvent {
public:
    using CallbackType = std::function<void(Args...)>;
    using ListenerId = std::uint64_t;

    ListenerId AddListener(CallbackType callback) {
        ListenerId id = nextListenerId++;
        callbacks.push_back({id, std::move(callback)});
        return id;
    }

    bool RemoveListener(ListenerId id) {
        auto oldSize = callbacks.size();
        callbacks.erase(
            std::remove_if(callbacks.begin(), callbacks.end(), [id](const ListenerEntry &entry) {
                return entry.id == id;
            }),
            callbacks.end());
        return callbacks.size() != oldSize;
    }

    void Invoke(Args... args) {
        for (auto& callback : callbacks) {
            callback.fn(args...);
        }
    }

private:
    struct ListenerEntry {
        ListenerId id;
        CallbackType fn;
    };

    std::vector<ListenerEntry> callbacks;
    ListenerId nextListenerId = 1;
};

#endif // COSMIC_EVENT_HPP
