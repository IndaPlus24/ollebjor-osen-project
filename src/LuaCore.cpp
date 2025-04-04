#include <lua.hpp>
#include <iostream>
#include <vector>
#include "LuaCore.hpp"
#include "PrimitiveLua.hpp"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

const std::string LuaCore::Version = "0.1.2";

namespace {
int luaGetVersion(lua_State* L) {
    lua_pushstring(L, LuaCore::Version.c_str());
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

void LuaCore::RegisterLuaClass(std::string name, const luaL_Reg methods[],
                               const luaL_Reg funcs[]) const {

    std::string mtName = name + ".Metatable";
    int ok = luaL_newmetatable(
        L, mtName.c_str()); // Pushes a new table onto the stack
    luaL_setfuncs(L, methods,
                  0);     // Register all methods in the array to the table
    lua_pushvalue(L, -1); // Pushes the metatable onto the stack again
    lua_setfield(L, -2, "__index"); // metatable.__index = metatable
    lua_pop(L, 1);

    lua_createtable(L, 0, 1);             // Create library table
    luaL_setmetatable(L, mtName.c_str()); // Set the metatable for the table)

    luaL_setfuncs(L, funcs,
                  0); // Register all methods in the array to the table
    lua_setglobal(
        L, name.c_str()); // Consume the top of the stack and make it a global
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
    // RegisterLuaClass(PrimitiveLua::luaName, PrimitiveLua::methods,
    // PrimitiveLua::functions);
    InitializePrimitive();
}

void LuaCore::pcall(int nargs, int nresults, int errfunc) const {
    if (lua_pcall(L, nargs, nresults, errfunc)) {
        std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
}

int print(lua_State* L) {
    PrimitiveLua* i = (PrimitiveLua*)luaL_checkudata(
        L, 1, PrimitiveLua::metatableName.c_str());
    luaL_argcheck(L, L != nullptr, 1, "'Primitive' expected in gc");
    std::cout << "Garbage collection" << std::endl;
    return 0;
}

int LuaCore::InitializePrimitive() {

    int ok =
        luaL_newmetatable(L, PrimitiveLua::metatableName
                                 .c_str()); // Pushes a new table onto the stack
    luaL_setfuncs(L, PrimitiveLua::methods,
                  0); // Register all methods in the array to the table

    lua_pushcfunction(L, PrimitiveLua::luaIndexPrimitive);
    lua_setfield(L, -2, "__index"); // metatable.__index = metatable

    lua_pushcfunction(L, print);
    lua_setfield(L, -2,
                 "__gc"); // metatable.__gc = PrimitiveLua::luaIndexPrimitive

    lua_pushcfunction(L, PrimitiveLua::luaNewIndexPrimitive);
    lua_setfield(L, -2,
                 "__newindex"); // metatable.__newindex =
                                // PrimitiveLua::luaNewIndexPrimitive

    lua_pop(L, 1);

    lua_createtable(L, 0, 1); // Create library table
    // luaL_setmetatable(L, PrimitiveLua::metatableName
    //                          .c_str()); // Set the metatable for the table)

    luaL_setfuncs(L, PrimitiveLua::functions,
                  0); // Register all methods in the array to the table
    lua_setglobal(
        L, PrimitiveLua::luaName
               .c_str()); // Consume the top of the stack and make it a global

    return 1;
}

void LuaCore::ExportVector3() const {

    // LuaType Vector3 = LuaCore::CreateType(L, "Vector3"); --Creates a new
    // LuaType class which handles the creation of the Vector3 class
    // Vector3.AddFunc("new", &Vector3::luaNew);
    // Vector3.AddMethod("GetPosition", &Vector3::luaGetPosition);
}

LuaCore::LuaCore() { L = luaL_newstate(); }
LuaCore::~LuaCore() {
    if (L) {
        lua_close(L);
    }
}
