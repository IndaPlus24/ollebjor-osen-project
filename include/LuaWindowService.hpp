#pragma once
#include <lua.hpp>
#include "LuaSignal.hpp"
#include "Renderer.hpp"
#include "SceneManager.hpp"
#include "LuaType.hpp"

class LuaWindowService {

  public:
    LuaWindowService() { this->Minimized = new LuaSignal(); };
    ~LuaWindowService() = default;

    static int luaSetTitle(lua_State* L) {
        LuaUtil::Get().CheckUserdata<LuaWindowService>(L, 1);
        const char* title = luaL_checkstring(L, 2);
        return 0;
    }
    static int luaMinimized(lua_State* L) {
        auto self = LuaUtil::Get().CheckUserdata<LuaWindowService>(L, 1);
        LuaUtil::Get().WrapAndPush(L, self->Minimized);
        return 1;
    }

    static void SetTitle(const std::string& title) {
        SceneManager::Get().GetRenderer().SetTitle(title);
    }

    LuaSignal* Minimized;
};