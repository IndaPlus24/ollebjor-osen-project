#include <lua.hpp>
#include <iostream>
#include "Lua.hpp"
using namespace std;

Lua::Lua() {
    L = luaL_newstate();
    luaL_openlibs(L);
}

Lua::~Lua() {
    if (L) {
        lua_close(L);
    }
}

void Lua::Run(string script) const{
    Prepare(script);
    pcall(0, 0, 0);
}

void Lua::Prepare(string script) const {
    if (luaL_loadfile(L, script.c_str())) {
        cerr << "Failed to prepare file: " << lua_tostring(L, -1)
                  << endl;
        lua_pop(L, 1);
        return;
    }
}

void Lua::SetGlobal(const string name, const string value) const {
    lua_pushstring(L, value.c_str());
    lua_setglobal(L, name.c_str());
}

string Lua::GetGlobal(string name) const {
    lua_getglobal(L, name.c_str());
    string global = lua_tostring(L, -1);
    lua_pop(L, 1);
    return global;
}

void Lua::Init() {

    //Prepend output from lua with "[Lua]:"

    return;
}

void Lua::pcall(int nargs, int nresults, int errfunc) const {
    if (lua_pcall(L, nargs, nresults, errfunc)) {
        cerr << "Error: " << lua_tostring(L, -1) << endl;
        lua_pop(L, 1);
    }
}