#pragma once
#include <lua.hpp>
#include <string>
#include "LuaWindowService.hpp"
#include <sol/sol.hpp>

// The Lua core helps with functions and utility surrounding lua.
class LuaCore {
  public:
    LuaCore();
    ~LuaCore();

    void Init();

    // Prepares and runs the specified Lua script.
    void Run(std::string path) const;
    // Sets a global variable in the Lua state. Currently only works with
    // strings.FIXME
    void SetGlobal(std::string name, std::string value) const;
    std::string GetGlobal(std::string name) const;

    void FireSignal(LuaSignal* signal) const;

    inline static const std::string Version = "0.1.3";

    // LuaService Instances
    LuaWindowService WindowService;

  private:
    static const struct luaL_Reg overrides[];

    void prepare(std::string path) const;
    void pcall(int narg, int nres, int errfunc) const;
    void registerGlobalFunction(lua_CFunction func, std::string luaFName) const;
    void overrideLuaLibFunctions() const;

    lua_State* L;
};
