#pragma once
#include <lua.hpp>
#include "Renderer.hpp"
#include "SceneManager.hpp"

class LuaWindow {

  private:
    LuaWindow() = default;
    ~LuaWindow() = default;

  public:
    static LuaWindow& Get() {
        static LuaWindow instance;
        return instance;
    }

    void SetTitle(std::string title) {
        SceneManager::Get().GetRenderer().SetTitle(title);
    }
};
