#pragma once
#include "lua/LuaEvent.hpp"
#include <sol/forward.hpp>
#include <sol/optional_implementation.hpp>
#include <sol/sol.hpp>
#include <sol/state_view.hpp>
#include <vector>

template <typename... Args> class Observer {
  public:
    virtual void onNotify(Args... args) = 0;
};

template <typename... Args> class Observable {
  private:
    std::vector<Observer<Args...>*> m_observers;

  public:
    void notify(Args... args) {
        for (const auto& observer : m_observers) {
            observer->onNotify(args...);
        }
    }

    void addObserver(Observer<Args...>* observer) {
        if (observer != nullptr) {
            m_observers.push_back(observer);
        }
    }

    void removeObserver(Observer<Args...>* observer) {
        m_observers.erase(
            std::remove(m_observers.begin(), m_observers.end(), observer),
            m_observers.end());
    }
};

template <typename... Args> class LuaObserver : public Observer<Args...> {
  public:
    LuaObserver(LuaEvent& event) : m_event(event) {};

    void onNotify(Args... args) override {
        if (m_event.hasCallback()) {
            m_event.getCallback()(args...);
        }
    }

  private:
    LuaEvent& m_event;
};