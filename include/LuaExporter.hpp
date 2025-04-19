#pragma once
#include <iostream>
#include <lua.hpp>
#include <string>
#include <vector>
#include "MetatableRegistry.hpp"

// Lua class is the base class for C++ classes that can be exported to Lua.
template <typename T> class LuaExporter {

  public:
    LuaExporter(lua_State* L, std::string name);
    ~LuaExporter();

    LuaExporter<T> Func(std::string name, lua_CFunction func, int nargs = 0);
    LuaExporter<T> Method(std::string name, lua_CFunction func, int nargs = 0);
    LuaExporter<T> Setter(std::string name, lua_CFunction setter);
    LuaExporter<T> Getter(std::string name, lua_CFunction getter);
    LuaExporter<T> Constructor(lua_CFunction constructor, int nargs = 0);
    LuaExporter<T> Meta(std::string name, lua_CFunction meta);
    void Export(bool isReferenceType = false,
                bool autoGC = true); // Export the class to Lua
    T* ExportAsSingleton();

  private:
    std::string name;
    lua_State* L;
    std::vector<luaL_Reg> methods;
    std::vector<luaL_Reg> funcs;
    std::vector<luaL_Reg> metamethods;
    std::vector<luaL_Reg> getters;
    std::vector<luaL_Reg> setters;
    void enableGetters(lua_State* L, std::string mtName);
    void enableSetters(lua_State* L, std::string mtName);
    void enableMethods(lua_State* L, std::string mtName);
    void createMetaSubTable(lua_State* L, std::vector<luaL_Reg>& funcs,
                            std::string name);
};

// Add template implementation in the header file

template <typename T>
LuaExporter<T>::LuaExporter(lua_State* L, std::string luaName) {
    this->name = luaName;
    this->L = L;
}

template <typename T> LuaExporter<T>::~LuaExporter() {
    // Destructor logic if needed
}

template <typename T>
LuaExporter<T> LuaExporter<T>::Func(std::string name, lua_CFunction func,
                                    int nargs) {
    luaL_Reg funcReg = {name.c_str(), func};
    funcs.push_back(funcReg);
    return *this;
}

template <typename T>
LuaExporter<T> LuaExporter<T>::Method(std::string name, lua_CFunction func,
                                      int nargs) {
    luaL_Reg method = {name.c_str(), func};
    methods.push_back(method);
    return *this;
}

template <typename T>
LuaExporter<T> LuaExporter<T>::Setter(std::string name, lua_CFunction setter) {
    luaL_Reg prop = {name.c_str(), setter};
    setters.push_back(prop);
    return *this;
}

template <typename T>
LuaExporter<T> LuaExporter<T>::Getter(std::string name, lua_CFunction getter) {
    luaL_Reg prop = {name.c_str(), getter};
    getters.push_back(prop);
    return *this;
}

//What does this do?
template <typename T>
LuaExporter<T> LuaExporter<T>::Constructor(lua_CFunction constFac, int nargs) {
    luaL_Reg func = {"new", constFac};
    funcs.push_back(func);
    return *this;
}

template <typename T>
LuaExporter<T> LuaExporter<T>::Meta(std::string name, lua_CFunction meta) {
    luaL_Reg func = {name.c_str(), meta};
    metamethods.push_back(func);
    return *this;
}

template <typename T> static lua_CFunction __index_wrapper() {
    return [](lua_State* L) -> int {
        const char* mtName =
            MetatableRegistry::Get().get_metatable_name<T>();
        T* obj = (T*)luaL_checkudata(L, 1, mtName);

        const char* key = luaL_checkstring(L, 2);
        int type = lua_type(L, 2);
        lua_pop(L, 1); // Pop the key off the stack

        if (type == LUA_TSTRING) {
            // std::cout << "It is a string: " << key << std::endl;
        } else {
            return luaL_error(L,
                              "Invalid key type, only strings are supported. "
                              "This might change in the future.");
        }

        luaL_getmetatable(L, mtName);
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
    };
}

template <typename T> static lua_CFunction __newindex_wrapper() {
    return [](lua_State* L) -> int {
        const char* mtName =
            MetatableRegistry::Get().get_metatable_name<T>();
        void* obj = (void*)luaL_checkudata(L, 1, mtName);

        const char* key = luaL_checkstring(L, 2);
        int type = lua_type(L, 2);
        lua_pop(L, 1); // Pop the key off the stack

        if (type == LUA_TSTRING) {
            // std::cout << "It is a string: " << key << std::endl;
        } else {
            return luaL_error(L,
                              "Invalid key type, only strings are supported. "
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
    };
}

// Helper function needed by the Export method
static bool has_field(lua_State* L, const char* field) {
    lua_getfield(L, -1, field);
    bool hasField = !lua_isnil(L, -1);
    lua_pop(L, 1);
    return hasField;
}

template <typename T>
void LuaExporter<T>::createMetaSubTable(lua_State* L,
                                        std::vector<luaL_Reg>& funcs,
                                        std::string name) {
    lua_newtable(L);
    luaL_setfuncs(L, funcs.data(), 0);
    lua_setfield(L, -2, name.c_str());
}

template <typename T>
void LuaExporter<T>::enableGetters(lua_State* L, std::string mtName) {
    luaL_getmetatable(L, mtName.c_str());
    createMetaSubTable(L, getters, "__get");
    lua_pop(L, 1); // Pop the metatable off the stack
}

template <typename T>
void LuaExporter<T>::enableSetters(lua_State* L, std::string mtName) {
    luaL_getmetatable(L, mtName.c_str());
    createMetaSubTable(L, setters, "__set");
    lua_pop(L, 1); // Pop the metatable off the stack
}

template <typename T>
void LuaExporter<T>::enableMethods(lua_State* L, std::string mtName) {
    luaL_getmetatable(L, mtName.c_str());
    createMetaSubTable(L, methods, "__methods");
    lua_pop(L, 1); // Pop the metatable off the stack
}

// Creates an instance of T and creates a type to match it.
// Then makes this instance a global variable in Lua with the name of the class
// TODO: return a pointer to the instance
template <typename T> T* LuaExporter<T>::ExportAsSingleton() {
    funcs.push_back({nullptr, nullptr});       // End of the functions array
    methods.push_back({nullptr, nullptr});     // End of the methods array
    metamethods.push_back({nullptr, nullptr}); // End of the metamethods array
    getters.push_back({nullptr, nullptr});     // End of the getters array
    setters.push_back({nullptr, nullptr});

    // Create a metatable with the __index set to itself
    MetatableRegistry::Get().register_metatable<T>(name + ".Metatable",
                                                        false, false);
    MetatableRegistry::Get().setup_metatable<T>(
        L); // Creates a default metatable with __name and __gc
    const char* mtName = MetatableRegistry::Get().get_metatable_name<T>();
    enableMethods(L, mtName); // Enable methods in the metatable
    enableGetters(L, mtName); // Enable getters in the metatable
    enableSetters(L, mtName); // Enable setters in the metatable
    luaL_getmetatable(L, mtName); // Push the metatable on the stack
    // Set normal metamethods directly to the metatable
    luaL_setfuncs(L, metamethods.data(), 0);
    
    // Sets the __index and __newindex metamethods to the metatable
    // if they are not already, manually set
    if (!has_field(L, "__index")) {
        lua_pushcfunction(L, __index_wrapper<T>());
        lua_setfield(L, -2, "__index");
    }
    if (!has_field(L, "__newindex")) {
        lua_pushcfunction(L, __newindex_wrapper<T>());
        lua_setfield(L, -2, "__newindex");
    }


    T* ins = MetatableRegistry::Get().create_and_push<T>(L);
    lua_setglobal(L, name.c_str()); // Set the userdata as a global variable "game"
    return ins;
}

template <typename T>
void LuaExporter<T>::Export(bool isReferenceType, bool autoGC) {
    funcs.push_back({nullptr, nullptr});       // End of the functions array
    methods.push_back({nullptr, nullptr});     // End of the methods array
    metamethods.push_back({nullptr, nullptr}); // End of the metamethods array
    getters.push_back({nullptr, nullptr});     // End of the getters array
    setters.push_back({nullptr, nullptr});

    MetatableRegistry::Get().register_metatable<T>(
        name + ".Metatable", isReferenceType, autoGC);
    MetatableRegistry::Get().setup_metatable<T>(
        L); // Creates a default metatable with __name and __gc
    const char* mtName = MetatableRegistry::Get().get_metatable_name<T>();
    enableMethods(L, mtName); // Enable methods in the metatable
    enableGetters(L, mtName); // Enable getters in the metatable
    enableSetters(L, mtName); // Enable setters in the metatable

    luaL_getmetatable(L, mtName); // Push the metatable on the stack
    // Set normal metamethods directly to the metatable
    luaL_setfuncs(L, metamethods.data(), 0);

    // Sets the __index and __newindex metamethods to the metatable
    // if they are not already, manually set
    if (!has_field(L, "__index")) {
        lua_pushcfunction(L, __index_wrapper<T>());
        lua_setfield(L, -2, "__index");
    }

    if (!has_field(L, "__newindex")) {
        lua_pushcfunction(L, __newindex_wrapper<T>());
        lua_setfield(L, -2, "__newindex");
    }

    lua_createtable(L, 0, 1); // Create library table
    luaL_setfuncs(L, funcs.data(),
                  1); // Register all methods in the array to the table
    // TODO: set metatable for library table so it cannot be modified

    lua_setglobal(
        L, name.c_str()); // Consume the top of the stack and make it a global
}