#pragma once
#include <lua.hpp>

// Lua class is the base class for C++ classes that can be exported to Lua.
class LuaClass {

  public:
    // The first pushed upvalue should always be the class instance itself.
    inline virtual int pushLuaUpvalues(lua_State* L) {
        lua_pushlightuserdata(L, this);
        return 0;
    };

    // Get the first arguemnt as a luaclass instance.
    static inline LuaClass* getInstance(lua_State* L) {
        return reinterpret_cast<LuaClass*>(
            lua_touserdata(L, lua_upvalueindex(1)));
    }
};
