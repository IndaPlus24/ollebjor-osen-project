#pragma once
#include <lua.hpp>
#include "LuaExporter.hpp"
#include "Renderer.hpp"
#include "SceneManager.hpp"
#include "LuaEvent.hpp"

class LuaWindowService {

  public:
    LuaWindowService() = default;
    ~LuaWindowService() = default;

    static int luaSetTitle(lua_State* L) {
        MetatableRegistry::Get().check_userdata<LuaWindowService>(L, 1);
        const char* title = luaL_checkstring(L, 2);
        SceneManager::Get().GetRenderer().SetTitle(title);
        return 0;
    }
    LuaEvent* Minimized;
};