#include <lua.hpp>
#include <iostream>
#include "LuaExporter.hpp"
#include "LuaCore.hpp"
#include "LuaVector3.hpp"
#include "LuaPrimitive.hpp"
#include "LuaWindowService.hpp"

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

void LuaCore::Run(std::string script) const {
    prepare(script);
    pcall(0, 0, 0);
}

void LuaCore::prepare(std::string script) const {
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

    LuaExporter<LuaWindowService> window(L, "Window");
    window.Method("SetTitle", LuaWindowService::luaSetTitle, 1)
        .ExportAsSingleton();

    LuaExporter<LuaVector3> vector3(L, "Vector3");
    vector3.Constructor(LuaVector3::luaNew, 3)
        .Method("Dot", LuaVector3::luaDot, 1)
        .Method("Cross", LuaVector3::luaCross, 1)
        .Method("GetLength", LuaVector3::luaGetLength, 0)
        .Getter("X", LuaVector3::luaGetX)
        .Getter("Y", LuaVector3::luaGetY)
        .Getter("Z", LuaVector3::luaGetZ)
        .Getter("len", LuaVector3::luaGetLength)
        .Getter("normalized", LuaVector3::luaNormalize)
        .Meta("__add", LuaVector3::lua__add)
        .Meta("__sub", LuaVector3::lua__sub)
        .Meta("__mul", LuaVector3::lua__mul)
        .Meta("__div", LuaVector3::lua__div)
        .Meta("__eq", LuaVector3::lua__eq)
        .Meta("__tostring", LuaVector3::lua__tostring)
        .Export(true, true);
    std::cout << __FILE_NAME__ << __LINE__ << std::endl;

    LuaExporter<LuaPrimitive> primitive(L, "Primitive");
    primitive.Constructor(LuaPrimitive::luaNew, 1)
        .Method("SetPosition", LuaPrimitive::luaSetPosition, 1)
        .Method("GetPosition", LuaPrimitive::luaGetPosition, 0)
        .Method("SetType", LuaPrimitive::luaSetType, 1)
        .Method("GetType", LuaPrimitive::luaGetType, 0)
        .Method("Destroy", LuaPrimitive::luaDestroy, 0)
        .Export(true, true);
    std::cout << __FILE_NAME__ << __LINE__ << std::endl;
}

void LuaCore::pcall(int nargs, int nresults, int errfunc) const {
    if (lua_pcall(L, nargs, nresults, errfunc)) {
        std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
}

// int print(lua_State* L) {
//     LuaPrimitive* i = (LuaPrimitive*)luaL_checkudata(
//         L, 1, LuaPrimitive::metatableName.c_str());
//     luaL_argcheck(L, L != nullptr, 1, "'Primitive' expected in gc");
//     std::cout << "Garbage collection" << std::endl;
//     return 0;
// }

LuaCore::LuaCore() { L = luaL_newstate(); }
LuaCore::~LuaCore() {
    if (L) {
        lua_close(L);
    }
}
