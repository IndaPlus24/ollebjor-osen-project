#include <lua.hpp>
#include <iostream>
#include <luaaa.hpp>
#include "Lua.hpp"
#include "PrimitiveLua.hpp"

const float Lua::Version = 0.1f;

namespace{
    int luaGetVersion(lua_State* L) {
        lua_pushnumber(L, Lua::Version);
        return 1;
    }
}
namespace{
    int luaPrintOverride(lua_State* L) {
        int n = lua_gettop(L); // number of arguments
        std::cout << "[Lua]: ";
        for (int i = 1; i <= n; i++) {
            std::cout << luaL_tolstring(L, i, nullptr) << "";
        }
        std::cout << std::endl;
        return 0;
    }
}

const luaL_Reg Lua::overrides[] = {
    {"print", luaPrintOverride},
    {nullptr, nullptr}
};

void Lua::overrideLuaLibFunctions() const {
    lua_getglobal(L, "_G");
    luaL_setfuncs(L, overrides, 0);
    lua_pop(L, 1);
}

void Lua::registerGlobalFunction(lua_CFunction func, std::string luaFName) const {
    lua_pushcfunction(L, func);
    lua_setglobal(L, luaFName.c_str());
}

void Lua::RegisterLuaClass(LuaClass& luaClass) const {
    lua_createtable(L, 0, luaClass.luaMethods().size() - 1); // Create a new table that is pushed onto stack
    luaL_setfuncs(L, luaClass.luaMethods().data(), 0); // Register all methods in the array to the table
    lua_setglobal(L, luaClass.luaName().c_str()); // Consume the top of the stack and make it a global
}

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
    luaL_openlibs(L);
    registerGlobalFunction(luaGetVersion, "Version");
    overrideLuaLibFunctions();
    
    //InitializePrimitives();

    return;
}

void Lua::pcall(int nargs, int nresults, int errfunc) const {
    if (lua_pcall(L, nargs, nresults, errfunc)) {
        std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
}

int Lua::InitializePrimitives() {
    std::string METATABLE_NAME = "Primitive.Metatable";

    std::vector<luaL_Reg> REGS = {
    };

    // Register the Primitive class in Lua
    glm::vec3* vector3 = new glm::vec3();
    glm::vec3** userdata = reinterpret_cast<glm::vec3**>(
        lua_newuserdatauv(L, sizeof(glm::vec3*), 0));
    *userdata = vector3;
    int type = luaL_getmetatable(
        L, METATABLE_NAME.c_str());
    if (type == LUA_TNIL)
    {
        lua_pop(L, 1);
        luaL_newmetatable(L, METATABLE_NAME.c_str());
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
        luaL_setfuncs(L, REGS.data(), 0);
    }

    lua_setmetatable(L, 1);

    return 1;

}
