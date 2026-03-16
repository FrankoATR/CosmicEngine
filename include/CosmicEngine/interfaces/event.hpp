#ifndef COSMIC_EVENT_HPP
#define COSMIC_EVENT_HPP

#include "ievent.hpp"
#include <vector>
#include <functional>

template<typename... Args>
class Event : public IEvent {
public:
    using CallbackType = std::function<void(Args...)>;

    void AddListener(CallbackType callback) {
        callbacks.push_back(callback);
    }

    void Invoke(Args... args) {
        for (auto& callback : callbacks) {
            callback(args...);
        }
    }

private:
    std::vector<CallbackType> callbacks;
};

#endif // COSMIC_EVENT_HPP
