#pragma once
#include <lua.hpp>
#include <luaaa.hpp>

using namespace std;
//The Lua class helps with functions and utility surrounding lua.

class Lua {
public:
    Lua();
    ~Lua();
    
    void Init();

    void Prepare(string path) const;
    void Run(string path) const;
    void SetGlobal(string name, string value) const;
    string GetGlobal(string name) const;

    template <typename T>
    void RegisterSingleton(string name, T instance) const;
    

private:

    void pcall(int narg, int nres, int errfunc) const;

    lua_State* L;
};