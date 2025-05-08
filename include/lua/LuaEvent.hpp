#pragma once
#include <functional>
#include <vector>
#include <memory>
#include <sol/sol.hpp>

class LuaConnection {
  public:
    LuaConnection(std::function<void()> disconnectFunc)
        : disconnectFunc(disconnectFunc) {}

    void Disconnect() {
        if (disconnectFunc)
            disconnectFunc();
        disconnectFunc = nullptr;
    }

  private:
    std::function<void()> disconnectFunc;
};

class LuaEvent {
  public:
    LuaEvent() = default;
    ~LuaEvent() = default;
    using Handler = std::function<void(sol::variadic_args)>;

    std::shared_ptr<LuaConnection> Connect(sol::function func) {
        auto handler = std::make_shared<Handler>(
            [func](sol::variadic_args args) { func(sol::as_args(args)); });
        handlers.push_back(handler);

        return std::make_shared<LuaConnection>([this, handler]() {
            auto it = std::find(handlers.begin(), handlers.end(), handler);
            if (it != handlers.end())
                handlers.erase(it);
        });
    }

    void Fire(sol::variadic_args args) {
        for (auto& handler : handlers) {
            if (handler)
                (*handler)(args);
        }
    }

  private:
    std::vector<std::shared_ptr<Handler>> handlers;
};