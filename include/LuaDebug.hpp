#pragma once
#include <lua.hpp>
#include <iostream>

class LuaDebug {
  public:
    inline static void PrintStack(lua_State* L, int level = 0) {
        int top = lua_gettop(L);
        std::cout << "Lua Stack (top -> bottom):\n";
        for (int i = top; i >= 1; --i) {
            int t = lua_type(L, i);
            std::cout << "[" << i << "] ";
            switch (t) {
            case LUA_TSTRING:
                std::cout << "string: " << lua_tostring(L, i);
                break;
            case LUA_TBOOLEAN:
                std::cout << "bool: "
                          << (lua_toboolean(L, i) ? "true" : "false");
                break;
            case LUA_TNUMBER:
                std::cout << "number: " << lua_tonumber(L, i);
                break;
            case LUA_TFUNCTION:
                std::cout << "function";
                break;
            case LUA_TTABLE:
                std::cout << "table";
                break;
            case LUA_TNIL:
                std::cout << "nil";
                break;
            default:
                std::cout << lua_typename(L, t);
                break;
            }
            std::cout << "\n";
        }
    }
};
