#pragma once
#include <lua.hpp>
#include <sol/as_args.hpp>
#include <sol/forward.hpp>
#include <sol/types.hpp>
#include "Renderer.hpp"
#include "SceneManager.hpp"
#include "lua/LuaEvent.hpp"

class LuaWindow {

  public:
    LuaWindow() = default;
    ~LuaWindow() = default;

    void SetTitle(std::string title) {
        SceneManager::Get().GetRenderer().SetTitle(title);
    }

    LuaEvent Minimized;
};
