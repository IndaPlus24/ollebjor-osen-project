#include "LuaExporter.hpp"
#include "lauxlib.h"
#include <lua.hpp>
#include <string>
#include <vector>
struct LuaClass {};

LuaExporter::LuaExporter(lua_State* L, std::string name) {
    this->name = name;
    this->metatableName = name + ".Metatable";
    this->L = L;
}

LuaExporter::~LuaExporter() {
    // Destructor logic if needed
}

LuaExporter LuaExporter::Method(std::string name, lua_CFunction func,
                                int nargs) {
    luaL_Reg method = {name.c_str(), func};
    methods.push_back(method);
    return *this;
}

LuaExporter LuaExporter::Methods(luaL_Reg methods[]) {
    for (int i = 0; methods[i].name != nullptr; i++) {
        this->methods.push_back(methods[i]);
    }
    return *this;
}

LuaExporter LuaExporter::Func(std::string name, lua_CFunction func, int nargs) {
    luaL_Reg funcReg = {name.c_str(), func};
    funcs.push_back(funcReg);
    return *this;
}

LuaExporter LuaExporter::Funcs(luaL_Reg funcs[]) {
    for (int i = 0; funcs[i].name != nullptr; i++) {
        this->funcs.push_back(funcs[i]);
    }
    return *this;
}

void LuaExporter::Export() {
    funcs.push_back({nullptr, nullptr});       // End of the functions array
    methods.push_back({nullptr, nullptr});     // End of the methods array
    metamethods.push_back({nullptr, nullptr}); // End of the metamethods array
    properties.push_back({nullptr, nullptr});  // End of the properties array

    int ok = luaL_newmetatable(
        L, metatableName.c_str()); // Pushes a new table onto the stack
    luaL_setfuncs(L, methods.data(),
                  0); // Register all methods in the array to the table
    luaL_setfuncs(L, metamethods.data(), 0);
    lua_pop(L, 1);

    lua_createtable(L, 0, 1); // Create library table
    luaL_setfuncs(L, funcs.data(),
                  0); // Register all methods in the array to the table
    // TODO: set metatable for library table so it cannot be overwritten

    lua_setglobal(
        L, name.c_str()); // Consume the top of the stack and make it a global
}