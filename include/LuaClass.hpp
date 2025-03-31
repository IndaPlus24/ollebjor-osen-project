#pragma once
#include <lua.hpp>

// Lua class is the base class for C++ classes that can be exported to Lua.
class LuaClass {

  public:
    const std::vector<luaL_Reg> luaMethods() const;
    const std::string luaName() const;
  protected:
    const std::string luaName;
    const std::vector<luaL_Reg> luaMethods;
};
