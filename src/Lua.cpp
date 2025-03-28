#include <lua.hpp>
#include <iostream>
#include <luaaa.hpp>
#include "Lua.hpp"
#include "PrimitiveLua.hpp"

Lua::Lua() {
    L = luaL_newstate();
}

Lua::~Lua() {
    if (L) {
        lua_close(L);
    }
}

void Lua::Run(std::string script) const{
    Prepare(script);
    pcall(0, 0, 0);
}

void Lua::Prepare(std::string script) const {
    if (luaL_loadfile(L, script.c_str())) {
        std::cerr << "Failed to prepare file: " << lua_tostring(L, -1)
                  << std::endl;
        lua_pop(L, 1);
        return;
    }
}

void Lua::SetGlobal(const std::string name, const std::string value) const {
    lua_pushstring(L, value.c_str());
    lua_setglobal(L, name.c_str());
}

std::string Lua::GetGlobal(std::string name) const {
    lua_getglobal(L, name.c_str());
    std::string global = lua_tostring(L, -1);
    lua_pop(L, 1);
    return global;
}

void Lua::Init() {

    //TODO: Prepend output from lua with "[Lua]:"
    luaL_openlibs(L);
    InitializePrimitives();

    return;
}

void Lua::pcall(int nargs, int nresults, int errfunc) const {
    if (lua_pcall(L, nargs, nresults, errfunc)) {
        std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
}

void Lua::InitializePrimitives() {
    // Register the Primitive class in Lua
    luaaa::LuaClass<PrimitiveLua> primitive(L, "Primitive");
    primitive.ctor<std::string>();
    // primitive.fun("setColor", &PrimitiveLua::SetColor);
    // primitive.fun("setPosition", &PrimitiveLua::SetPosition);
    // primitive.fun("getColor", &PrimitiveLua::GetColor);
    // primitive.fun("getPosition", &PrimitiveLua::GetPosition);
    primitive.fun("getName", &PrimitiveLua::GetName);
    primitive.get("name", &PrimitiveLua::GetName);
    // primitive.set("color", &PrimitiveLua::SetColor);
    // primitive.get("color", &PrimitiveLua::GetColor);
    // primitive.set("position", &PrimitiveLua::SetPosition);
    // primitive.get("position", &PrimitiveLua::GetPosition);
}
