#ifndef EVENT_HPP
#define EVENT_HPP

#include "IEvent.hpp"
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

#endif // EVENT_HPP
