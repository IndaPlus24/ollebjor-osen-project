#include "LuaExporter.hpp"
#include "lua.h"
#include <iostream>
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

LuaExporter LuaExporter::Getter(std::string name, lua_CFunction getter) {
    luaL_Reg prop = {name.c_str(), getter};
    getters.push_back(prop);
    return *this;
}

int __index(lua_State* L) {
    int i = lua_upvalueindex(1);
    const char* mtName = lua_tostring(L, i);
    void* vec = (void*)luaL_checkudata(L, 1, mtName);

    const char* key = luaL_checkstring(L, 2);
    int type = lua_type(L, 2);
    lua_pop(L, 1); // Pop the key off the stack

    if (type == LUA_TSTRING) {
        // std::cout << "It is a string: " << key << std::endl;
    } else {
        return luaL_error(L, "Invalid key type, only strings are supported. "
                             "This might change in the future.");
    }

    lua_getmetatable(L, 1);
    lua_getfield(L, -1, "__get");
    lua_getfield(L, -1, key);

    // First check __get
    if (!lua_isnil(L, -1)) {
        // We have the getter function on the top of the stack
        lua_remove(L, -2);
        lua_remove(L, -2);
        lua_insert(L, -2);
        int status = lua_pcall(L, 1, 1, 0);
        if (status != LUA_OK) {
            const char* error = lua_tostring(L, -1);
            std::cerr << "Error in __index: " << error << std::endl;
            lua_pop(L, 1); // Pop the error message off the stack
            return 0;
        }
        return 1;
    }

    lua_pop(L, 2); // Pop the nil value and __get of the stack
    // Then check __methods
    lua_getfield(L, -1, "__methods");
    lua_getfield(L, -1, key);
    return 1;
}

int __newindex(lua_State* L) {
    int i = lua_upvalueindex(1);
    const char* mtName = lua_tostring(L, i);
    void* vec = (void*)luaL_checkudata(L, 1, mtName);

    const char* key = luaL_checkstring(L, 2);
    int type = lua_type(L, 2);
    lua_pop(L, 1); // Pop the key off the stack

    if (type == LUA_TSTRING) {
        // std::cout << "It is a string: " << key << std::endl;
    } else {
        return luaL_error(L, "Invalid key type, only strings are supported. "
                             "This might change in the future.");
    }

    lua_getmetatable(L, 1);
    lua_getfield(L, -1, "__set");
    lua_getfield(L, -1, key);

    // First check __set
    if (!lua_isnil(L, -1)) {
        // We have the setter function on the top of the stack
        lua_remove(L, -2);
        lua_remove(L, -2);
        lua_insert(L, -2);
        int status = lua_pcall(L, 1, 1, 0);
        if (status != LUA_OK) {
            const char* error = lua_tostring(L, -1);
            std::cerr << "Error in __newindex: " << error << std::endl;
            lua_pop(L, 1); // Pop the error message off the stack
        }
        return 0;
    }
    return 1;
}

void LuaExporter::Export() {
    funcs.push_back({nullptr, nullptr});       // End of the functions array
    methods.push_back({nullptr, nullptr});     // End of the methods array
    metamethods.push_back({nullptr, nullptr}); // End of the metamethods array
    getters.push_back({nullptr, nullptr});     // End of the getters array

    // Push a new (meta)table onto the stack
    int ok = luaL_newmetatable(L, metatableName.c_str());

    // Create a new table for the __methods field OBS:__methods is not a
    // metamethod
    // metatable.__methods = methods
    lua_newtable(L);
    lua_pushstring(L, metatableName.c_str());
    luaL_setfuncs(L, methods.data(), 1);
    lua_setfield(L, -2, "__methods");

    // metatable.__get = getters
    lua_newtable(L);
    lua_pushstring(L, metatableName.c_str());
    luaL_setfuncs(L, getters.data(), 1);
    lua_setfield(L, -2, "__get");

    // metatable.__set = setters
    lua_newtable(L);
    lua_pushstring(L, metatableName.c_str());
    luaL_setfuncs(L, getters.data(), 1);
    lua_setfield(L, -2, "__set");

    // Set normal metamethods directly to the metatable
    luaL_setfuncs(L, metamethods.data(), 0);

    // Sets the __index and __newindex metamethods to the metatable
    // if they are not already, manually set
    lua_getfield(L, -1, "__index");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_pushstring(L, metatableName.c_str());
        lua_pushcclosure(L, __index, 1);
        lua_setfield(L, -2, "__index");
    } else {
        lua_pop(L, 2); // Pop the metatable and the function value off the stack
    }

    lua_getfield(L, -1, "__newindex");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_pushstring(L, metatableName.c_str());
        lua_pushcclosure(L, __newindex, 1);
        lua_setfield(L, -2, "__newindex");
        lua_pop(L, 1);  
    }else{
        lua_pop(L, 2);
    }

    lua_createtable(L, 0, 1); // Create library table
    lua_pushstring(L,
                   metatableName.c_str()); // Pushes the name of the metatable
    luaL_setfuncs(L, funcs.data(),
                  1); // Register all methods in the array to the table
    // TODO: set metatable for library table so it cannot be overwritten

    lua_setglobal(
        L, name.c_str()); // Consume the top of the stack and make it a global
}