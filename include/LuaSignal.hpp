#pragma once
#include <cstddef>
#include <iostream>
#include <lua.hpp>
#include "LuaDebug.hpp"
#include "LuaType.hpp"
#include <set>

class LuaSignal {
  public:
    LuaSignal() = default;
    static int luaOnReceive(lua_State* L) {
        LuaSignal* self = LuaUtil::Get().CheckUserdata<LuaSignal>(L, 1);
        luaL_checktype(L, 2, LUA_TFUNCTION);

        // Store the function in the registry (it pops it from the stack)
        size_t ref = luaL_ref(L, LUA_REGISTRYINDEX);

        auto pair = self->m_callbacksRefs.insert(ref);
        if (!pair.second) {
            std::cerr << "Error: Callback already exists." << std::endl;
            luaL_unref(L, LUA_REGISTRYINDEX, ref); // Unref the function
            return 0;
        }

        // Push a function that allows removing the callback
        lua_pushcfunction(L, [](lua_State* L) -> int {
            // Get the ref from closure
            int ref = lua_tointeger(L, lua_upvalueindex(1));

            // Get self from closure
            LuaSignal* self =
                static_cast<LuaSignal*>(lua_touserdata(L, lua_upvalueindex(2)));

            // Remove from callbacks set
            if (self && self->m_callbacksRefs.erase(ref) > 0) {
                // Unref the function in registry
                luaL_unref(L, LUA_REGISTRYINDEX, ref);
            }

            return 0;
        });

        // Push ref and self as upvalues for the disconnection function
        lua_pushinteger(L, ref);
        lua_pushlightuserdata(L, self);
        lua_pushcclosure(L, lua_tocfunction(L, -3), 2);

        // Remove the raw C function we used to create the closure
        lua_remove(L, -2);

        return 1;
    }

    static int luaSend(lua_State* L) {
        LuaSignal* self = LuaUtil::Get().CheckUserdata<LuaSignal>(L, 1);
        int n_args = lua_gettop(L) - 1; // Subtract 1 for self

        // Process each callback one at a time
        for (size_t ref : self->m_callbacksRefs) {
            // Get the function from the registry
            lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

            // Push copies of all arguments for this callback
            for (int arg = 2; arg <= n_args + 1; arg++) {
                lua_pushvalue(L, arg);
            }

            // Call the function with the arguments
            if (lua_pcall(L, n_args, 0, 0) != LUA_OK) {
                std::cerr << "Error calling Lua function: "
                          << lua_tostring(L, -1) << std::endl;
                lua_pop(L, 1); // Pop the error message
            }
        }
        return 0;
    }

    static int luaNew(lua_State* L) {
        LuaUtil::Get().CreateAndPush<LuaSignal>(L);
        return 1;
    }

  private:
    std::set<size_t> m_callbacksRefs;
};