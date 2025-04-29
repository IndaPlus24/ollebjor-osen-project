#pragma once
#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include <any>
#include <iostream>

struct EventHandle {
    size_t id;        // Unique ID for the listener
    std::string type; // Type of event the listener is interested in
};

class EventDispatcher {

  public:
    using Listener = std::function<void(const std::any&)>;

    static EventDispatcher& Get() {
        static EventDispatcher instance; // Guaranteed to be destroyed.
        return instance;                 // Instantiated on first use.
    }
    // Delete copy constructor and assignment operator
    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;

    EventHandle AddListener(std::string event, Listener listener) {
        size_t id = _nextListenerId++;
        m_listeners[event].emplace_back(id, listener);
        return {id, event};
    };

    void DispatchEvent(std::string event, const std::any& payload) {
        auto it = m_listeners.find(event);
        if (it != m_listeners.end()) {
            for (const auto& pair : it->second) {
                std::cout << "Dispatching event: " << event << std::endl;
                std::any_cast<Listener>(pair.second)(payload);
                std::cout << "Event dispatched" << std::endl;
            }
        }
    };

    void DispatchEvent(std::string event) { DispatchEvent(event, std::any{}); };

    void RemoveListener(EventHandle handle) {
        auto it = m_listeners.find(handle.type);
        if (it != m_listeners.end()) {
            auto& listeners = it->second;
            listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
                                           [handle](const auto& pair) {
                                               return pair.first == handle.id;
                                           }),
                            listeners.end());
        }
    };

    void RemoveAllListeners() { m_listeners.clear(); };

  private:
    EventDispatcher() = default;
    std::unordered_map<std::string, std::vector<std::pair<size_t, std::any>>>
        m_listeners;
    size_t _nextListenerId = 0;
};