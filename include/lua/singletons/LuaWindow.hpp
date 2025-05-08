#pragma once
#include <lua.hpp>
#include <sol/as_args.hpp>
#include <sol/types.hpp>
#include "Renderer.hpp"
#include "SceneManager.hpp"
#include "lua/LuaEvent.hpp"

class LuaWindow {

  private:
    LuaWindow() = default;
    ~LuaWindow() = default;
    LuaEvent m_update;

  public:
    static LuaWindow& Get() {
        static LuaWindow instance;
        return instance;
    }

    void SetTitle(std::string title) {
        SceneManager::Get().GetRenderer().SetTitle(title);
    }

    LuaEvent& GetUpdateEvent() { return m_update; }
    void FireUpdateEVent(float deltaTime) {
        sol::variadic_args args;
        args.push();

        m_update.Fire(args);
        
    }
};
