#pragma once
#include "bx/debug.h"
#include <iostream>
#include <optional>
#include <sol/forward.hpp>
#include <sol/sol.hpp>
#include <sol/state_handling.hpp>
#include <sol/state_view.hpp>
#include <sol/types.hpp>

class LuaEvent {
  private:
    std::optional<sol::protected_function> m_callback;

  public:
    LuaEvent() = default;
    ~LuaEvent() = default;
    bool hasCallback() const { return m_callback.has_value(); }

    sol::protected_function getCallback() const { return m_callback.value(); }

    void setCallback(sol::protected_function fn) { m_callback = std::move(fn); }

    void clearCallback() {
        if (hasCallback()) {
            m_callback->reset();
        }
    }

    template <typename... Args> void runCallback(Args... args) {
        if (hasCallback()) {
            auto result = getCallback()(args...);
            if (!result.valid()) {
                sol::error err = result;
                std::cerr << "LuaCore::Run: Error running callback2: "
                          << err.what() << "\n";
            }
        }
    }
};