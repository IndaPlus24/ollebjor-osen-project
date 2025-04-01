#pragma once
#include <array>
#include <cstddef>
#include <lua.hpp> // Ensure LuaClass is defined in this header file
#include "PrimitiveLua.hpp"
#include "lauxlib.h"
#include <string>
#include <vector>

#define NUM_LUA_EXPORTS 1

//The Lua class helps with functions and utility surrounding lua.

struct luaExport {
    std::string name;
    const luaL_Reg methods[];
};

class LuaCore {
public:
    LuaCore();
    ~LuaCore();
    
    void Init();

    void Run(std::string path) const;
    void SetGlobal(std::string name, std::string value) const;
    std::string GetGlobal(std::string name) const;
    void RegisterLuaClass(std::string name, const luaL_Reg methods[], size_t methodCount) const;

    static const float Version;
    
private:
    static const struct luaL_Reg overrides[];
    int InitializePrimitives();
    void Prepare(std::string path) const;
    void pcall(int narg, int nres, int errfunc) const;
    void registerGlobalFunction(lua_CFunction func, std::string luaFName) const;
    void overrideLuaLibFunctions() const;

    static luaExport luaExports[];

    lua_State* L;
};