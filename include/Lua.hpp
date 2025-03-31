#pragma once
#include <lua.hpp>
#include <luaaa.hpp>
#include "LuaClass.hpp"

using namespace std;
//The Lua class helps with functions and utility surrounding lua.

class Lua {
public:
    Lua();
    ~Lua();
    
    void Init();

    void Run(string path) const;
    void SetGlobal(string name, string value) const;
    string GetGlobal(string name) const;
    void RegisterLuaClass(LuaClass& luaClass) const;

    static const float Version;
    
private:
    static const struct luaL_Reg overrides[];
    int InitializePrimitives();
    void Prepare(string path) const;
    void pcall(int narg, int nres, int errfunc) const;
    void registerGlobalFunction(lua_CFunction func, std::string luaFName) const;
    void overrideLuaLibFunctions() const;

    lua_State* L;
};