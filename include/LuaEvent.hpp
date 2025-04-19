#pragma once
#include "EventDispatcher.hpp"
#include "LuaExporter.hpp"
#include <SDL_events.h>
#include <lua.hpp>
#include <string>
#include "LuaDebug.hpp"
#include "lauxlib.h"
#include "lua.h"

// Lua Event is the class that will be used to create events in Lua
//  It can be used to create events that can be triggered from C++
//  and can be connected to Lua functions or create custom ones for gameplay
//  logic

class LuaEvent {
  public:
    LuaEvent(std::string eventName) : m_eventName(eventName) {};
    ~LuaEvent() { std::cout << "LuaEvent destroyed" << std::endl; };

    static int luaConnect(lua_State* L) {
        LuaEvent* self =
            MetatableRegistry::Get().check_userdata<LuaEvent>(L, 1);
        luaL_checktype(L, 2, LUA_TFUNCTION);
        // Store it in the registry (it pops it from the stack)
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);

        // Add it as a listener
        EventHandle handle = EventDispatcher::Get().AddListener(
            self->m_eventName, [L, ref](const std::any& payload) {
                // Push it onto the stack
                lua_rawgeti(L, LUA_REGISTRYINDEX, ref);

                // Run the function
                if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
                    std::cerr
                        << "Error calling Lua function: " << lua_tostring(L, -1)
                        << std::endl;
                    lua_pop(L, 1); // Pop the error message from the stack
                }
            });

        lua_pushinteger(L, handle.id);
        lua_pushstring(L, self->m_eventName.c_str());
        lua_pushinteger(L, ref);
        lua_pushcclosure(
            L,
            [](lua_State* L) -> int {
                // Get first upvalue "id"
                size_t id = lua_tointeger(L, lua_upvalueindex(1));
                // Get second upvalue "eventName"
                std::string eventName = lua_tostring(L, lua_upvalueindex(2));
                // Get third upvalue "ref"
                size_t ref = lua_tointeger(L, lua_upvalueindex(3));
                EventHandle handle{id, eventName};
                EventDispatcher::Get().RemoveListener(handle);
                luaL_unref(L, LUA_REGISTRYINDEX, ref);
                return 0;
            },
            2); // 2 upvalues: id and eventName

        return 1;
    };

  private:
    std::string m_eventName;
};