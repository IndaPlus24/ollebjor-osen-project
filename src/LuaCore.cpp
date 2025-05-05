#include <lua.hpp>
#include <iostream>
#include "LuaCore.hpp"
#include "LuaVector3.hpp"
#include "LuaPrimitive.hpp"
#include "LuaSignal.hpp"
#include "LuaType.hpp"
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
// Sends a LuaSignal without any arguments to the Lua function
void LuaCore::FireSignal(LuaSignal* signal) const {
    LuaUtil::Get().WrapAndPush(L, signal);
    LuaSignal::luaSend(L);
    lua_pop(L, 1); // Pop the signal from the stack
};

void LuaCore::Init() {
    luaL_openlibs(L);
    registerGlobalFunction(luaGetVersion, "Version");
    overrideLuaLibFunctions();

    LuaType<LuaSignal> signalType(L, "Signal", true);
    signalType.AddMethod("Send", LuaSignal::luaSend)
        .AddMethod("OnReceive", LuaSignal::luaOnReceive)
        .MakeClass(LuaSignal::luaNew);

    LuaType<LuaWindowService> window(L, "Window", true, false);
    window.AddMethod("SetTitle", LuaWindowService::luaSetTitle)
        .AddProperty("Minimized", LuaWindowService::luaMinimized, nullptr)
        .MakeSingleton(&WindowService);

    LuaType<LuaVector3> vector3(L, "Vector3", true);
    vector3.AddMethod("Dot", LuaVector3::luaDot)
        .AddMethod("Cross", LuaVector3::luaCross)
        .AddMethod("GetLength", LuaVector3::luaGetLength)
        .AddProperty("X", LuaVector3::luaGetX, nullptr)
        .AddProperty("Y", LuaVector3::luaGetY, nullptr)
        .AddProperty("Z", LuaVector3::luaGetZ, nullptr)
        .AddProperty("len", LuaVector3::luaGetLength, nullptr)
        .AddProperty("normalized", LuaVector3::luaNormalize, nullptr)
        .AddMetaMethod("__add", LuaVector3::lua__add)
        .AddMetaMethod("__sub", LuaVector3::lua__sub)
        .AddMetaMethod("__mul", LuaVector3::lua__mul)
        .AddMetaMethod("__div", LuaVector3::lua__div)
        .AddMetaMethod("__eq", LuaVector3::lua__eq)
        .AddMetaMethod("__tostring", LuaVector3::lua__tostring)
        .MakeClass(LuaVector3::luaNew);

    LuaType<LuaPrimitive> primitive(L, "Primitive", true);
    primitive.AddMethod("SetPosition", LuaPrimitive::luaSetPosition)
        .AddMethod("GetPosition", LuaPrimitive::luaGetPosition)
        .AddMethod("SetType", LuaPrimitive::luaSetType)
        .AddMethod("GetType", LuaPrimitive::luaGetType)
        .AddMethod("Destroy", LuaPrimitive::luaDestroy)
        .MakeClass(LuaPrimitive::luaNew);
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

LuaCore::LuaCore() : L(luaL_newstate()), WindowService() {}
LuaCore::~LuaCore() {
    if (L) {
        lua_close(L);
    }
}
