#include <array>
#include <cstdlib>
#include <lua.hpp>
#include <iostream>
#include <vector>
#include "LuaCore.hpp"
#include "PrimitiveLua.hpp"
#include "lauxlib.h"
#include "LuaClass.hpp"
#include "lua.h"

const float LuaCore::Version = 0.1f;

namespace {
int luaGetVersion(lua_State* L) {
    lua_pushnumber(L, LuaCore::Version);
    return 1;
}
} // namespace
namespace {
int luaPrintOverride(lua_State* L) {
    int n = lua_gettop(L); // number of arguments
    std::cout << "[Lua]: ";
    for (int i = 1; i <= n; i++) {
        std::cout << luaL_tolstring(L, i, nullptr) << " ";
    }
    std::cout << std::endl;
    return 0;
}
} // namespace

const luaL_Reg LuaCore::overrides[] = {{"print", luaPrintOverride},
                                       {nullptr, nullptr}};

void LuaCore::overrideLuaLibFunctions() const {
    lua_getglobal(L, "_G");
    luaL_setfuncs(L, overrides, 0);
    lua_pop(L, 1);
}

void LuaCore::registerGlobalFunction(lua_CFunction func,
                                     std::string luaFName) const {
    lua_pushcfunction(L, func);
    lua_setglobal(L, luaFName.c_str());
}

void LuaCore::RegisterLuaClass(std::string name, const luaL_Reg methods[], size_t methodCount) const {
    lua_createtable(L, 0, methodCount); // Create table
    luaL_setfuncs(L, methods, 0); // Register all methods in the array to the table
    lua_setglobal(L, name.c_str()); // Consume the top of the stack and make it a global
}

void LuaCore::Run(std::string script) const {
    Prepare(script);
    pcall(0, 0, 0);
}

void LuaCore::Prepare(std::string script) const {
    if (luaL_loadfile(L, script.c_str())) {
        std::cerr << "Failed to prepare file: " << lua_tostring(L, -1)
                  << std::endl;
        lua_pop(L, 1);
        return;
    }
}

void LuaCore::SetGlobal(const std::string name, const std::string value) const {
    lua_pushstring(L, value.c_str());
    lua_setglobal(L, name.c_str());
}

std::string LuaCore::GetGlobal(std::string name) const {
    lua_getglobal(L, name.c_str());
    std::string global = lua_tostring(L, -1);
    lua_pop(L, 1);
    return global;
}

void LuaCore::Init() {
    luaL_openlibs(L);
    registerGlobalFunction(luaGetVersion, "Version");
    overrideLuaLibFunctions();
    RegisterLuaClass("Primitive", PrimitiveLua::methods, PRIMITIVE_METHOD_COUNT + 1);
    // InitializePrimitives();
}

void LuaCore::pcall(int nargs, int nresults, int errfunc) const {
    if (lua_pcall(L, nargs, nresults, errfunc)) {
        std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
}

int LuaCore::InitializePrimitives() {
    std::string METATABLE_NAME = "Primitive.Metatable";

    std::vector<luaL_Reg> REGS = {};

    // Register the Primitive class in Lua
    glm::vec3* vector3 = new glm::vec3();
    glm::vec3** userdata = reinterpret_cast<glm::vec3**>(
        lua_newuserdatauv(L, sizeof(glm::vec3*), 0));
    *userdata = vector3;
    int type = luaL_getmetatable(L, METATABLE_NAME.c_str());
    if (type == LUA_TNIL) {
        lua_pop(L, 1);
        luaL_newmetatable(L, METATABLE_NAME.c_str());
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        luaL_setfuncs(L, REGS.data(), 0);
    }

    lua_setmetatable(L, 1);

    return 1;
}

LuaCore::LuaCore() { L = luaL_newstate(); }
LuaCore::~LuaCore() {
    if (L) {
        lua_close(L);
    }
}
