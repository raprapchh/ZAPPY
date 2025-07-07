#pragma once
#include "Event.hpp"
#include <functional>
#include <list>
#include <unordered_map>
#include <typeindex>
namespace zappy {
class EventDispatcher {
public:
    using EventHandler = std::function<void(const Event&)>;
    template<typename T>
    void subscribe(std::function<void(const T&)> listener) {
        EventHandler handler = [listener](const Event& event) {
            listener(static_cast<const T&>(event));
        };
        listeners_[std::type_index(typeid(T))].push_back(handler);
    }
    void post(const Event& event) {
        auto it = listeners_.find(std::type_index(typeid(event)));
        if (it != listeners_.end()) {
            for (const auto& handler : it->second) {
                handler(event);
            }
        }
    }
private:
    std::unordered_map<std::type_index, std::list<EventHandler>> listeners_;
};
}
