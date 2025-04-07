#pragma once
#include <string>
#include <lua.hpp>
#include <vector>

// Lua class is the base class for C++ classes that can be exported to Lua.
class LuaExporter {

  public:
    LuaExporter(lua_State* L, std::string name);
    ~LuaExporter();

    LuaExporter Func(std::string name, lua_CFunction func, int nargs = 0);
    LuaExporter Funcs(luaL_Reg funcs[]);
    LuaExporter Method(std::string name, lua_CFunction func, int nargs = 0);
    LuaExporter Methods(luaL_Reg methods[]);
    // LuaExporter Property(std::string name, lua_CFunction getter, lua_CFunction setter);
    // LuaExporter Setter(std::string name, lua_CFunction setter);
    // LuaExporter Getter(std::string name, lua_CFunction getter);
    // LuaExporter Constructor(std::string name, int nargs = 0);
    void Export();

  private:
    std::string name; // TODO: Make enum
    std::string metatableName;
    lua_State* L;
    std::vector<luaL_Reg> methods;
    std::vector<luaL_Reg> funcs;
    std::vector<luaL_Reg> metamethods;
    std::vector<luaL_Reg> properties;
};
