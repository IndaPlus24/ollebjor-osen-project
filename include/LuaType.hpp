#pragma once
#include <lua.hpp>
#include <iostream>
#include <ostream>
#include <string>
#include "LuaExporter.hpp"
#include "LuaUtil.hpp"
#include "LuaDebug.hpp"

#define NO_GETTER [](lua_State* L) -> int { return 0; }
#define NO_SETTER [](lua_State* L) -> int { return 0; }

template <typename T> class LuaType {
  public:
    LuaType(lua_State* L, std::string name, bool is_reference_type = false, bool auto_gc = true) : L(L) {
        LuaUtil::Get().NewType<T>(L, name, is_reference_type, auto_gc);
        this->metatable_name = LuaUtil::Get().GetTypeName<T>();
    };
    ~LuaType() = default;

    LuaType& AddMethod(std::string name, lua_CFunction func) {
        push_metatable();
        lua_getfield(L, -1, "__methods");
        lua_pushcfunction(L, func);
        lua_setfield(L, -2, name.c_str());
        lua_pop(L, 2); // Pop the metatable and __methods off the stack
        return *this;
    }
    // TODO: Make all metamethods into an enumtype and use that instead of
    // strings
    LuaType& AddMetaMethod(std::string name, lua_CFunction func) {
        push_metatable();
        lua_pushcfunction(L, func);
        lua_setfield(L, -2, name.c_str());
        lua_pop(L, 1); // Pop the metatable off the stack
        return *this;
    }
    LuaType& AddProperty(std::string name, lua_CFunction getter,
                         lua_CFunction setter) {
        push_metatable();
        lua_getfield(L, -1, "__get");
        lua_pushcfunction(L, getter);
        lua_setfield(L, -2, name.c_str());
        lua_pop(L, 1); // Pop __get off the stack

        lua_getfield(L, -1, "__set");
        lua_pushcfunction(L, setter);
        lua_setfield(L, -2, name.c_str());
        lua_pop(L, 2); // Pop __set and the metatable off the stack
        return *this;
    }

    void MakeClass(lua_CFunction constructor) {
        allow_getters();
        allow_setters();

        // Create a library table for the class with the Library metatable
        lua_createtable(L, 0, 1); // Create library table

        LuaUtil::Get().PushLibraryMetatable(L);
        lua_setmetatable(L, -2); // Set the metatable for the class

        lua_pushcfunction(L, constructor);
        lua_setfield(L, -2, "new"); // Set the constructor function

        lua_setglobal(L, metatable_name); // Set the class table as a global
    };
    void MakeSingleton(T* instance) {
        allow_getters();
        allow_setters();
        std::cout << "allowing complete!" << std::endl;
        LuaUtil::Get().WrapAndPush(
            L, instance); // Wrap the instance and push it to the stack
        std::cout << "pushed!" << std::endl;
        lua_setglobal(L, metatable_name);
    };

  private:
    lua_State* L;
    const char* metatable_name;
    void push_metatable() {
        luaL_getmetatable(L, metatable_name);
        if (lua_isnil(L, -1)) {
            throw std::runtime_error("Metatable not found for type " +
                                     std::string(metatable_name));
            lua_pop(L, 1);
        }
    }
    void allow_getters() {
        push_metatable();
        lua_CFunction fn = LuaType<T>::__index_wrapper();
        lua_pushcfunction(L, fn);
        lua_setfield(L, -2, "__index"); // Pop the metatable off the stack
        lua_pop(L, 1);
    }
    void allow_setters() {
        push_metatable();
        lua_CFunction fn = LuaType<T>::__newindex_wrapper();
        lua_pushcfunction(L, fn);
        lua_setfield(L, -2, "__newindex"); // Pop the metatable off the stack
        lua_pop(L, 1);
    }

    static lua_CFunction __index_wrapper() {
        return [](lua_State* L) -> int {
            T* obj = LuaUtil::Get().CheckUserdata<T>(L, 1);
            const char* index = luaL_checkstring(L, 2);
            lua_pop(L, 1); // Pop the key off the stack

            const char* mtName = LuaUtil::Get().GetTypeName<T>();
            luaL_getmetatable(L, mtName);

            lua_getfield(L, -1, "__get");
            lua_getfield(L, -1, index);
            if (lua_isnil(L, -1)) {
                // We have no getter function, check __methods
                lua_pop(L, 2); // Pop nil and __get
                lua_getfield(L, -1, "__methods");
                lua_getfield(L, -1, index);
                return 1;
            }

            // We have the getter function on the top of the stack, we need to
            // call it with self as the first argument and no other arguments
            auto getter = lua_tocfunction(L, -1);
            lua_pop(L, 3); // Pop the function, __get and the metatable off the stack
            lua_pushcfunction(L, getter);
            lua_insert(L, -2); // Move the getter one step down
            if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
                const char* error = lua_tostring(L, -1);
                std::cerr << "Error in __index: " << error << std::endl;
                lua_pop(L, 1); // Pop the error message off the stack
                return 0;
            }
            return 1;
        };
    }

    static lua_CFunction __newindex_wrapper() {

        return [](lua_State* L) -> int {
            T* obj = LuaUtil::Get().CheckUserdata<T>(L, 1);
            const char* index = luaL_checkstring(L, 2);
            lua_pop(L, 1); // Pop the key off the stack

            const char* mtName = LuaUtil::Get().GetTypeName<T>();
            luaL_getmetatable(L, mtName);
            lua_getfield(L, -1, "__set");
            lua_getfield(L, -1, index);
            if (lua_isnil(L, -1)) {
                return 0;
            }

            auto setter = lua_tocfunction(L, -1);
            lua_pop(L, 3); // Pop the function, __set and the metatable off the stack
            lua_pushcfunction(L, setter);
            lua_insert(L, -2); // Move the setter one step down
            if (lua_pcall(L, 1, 0, 0) != LUA_OK) {
                const char* error = lua_tostring(L, -1);
                std::cerr << "Error in __newindex: " << error << std::endl;
                lua_pop(L, 1); // Pop the error message off the stack
            }
            return 0;
        };
    }
};