#include "LuaDebug.hpp"
#include "lua.h"
#include <iostream>
#include <typeindex>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <lua.hpp>

class LuaUtil {
  private:
    // Private constructor prevents external instantiation
    LuaUtil() {}

    // Store mapping between C++ types and Lua metatable names
    std::unordered_map<std::type_index, std::string> m_typeRegistry;
    // Store additional type metadata
    struct TypeMetadata {
        bool is_reference_type =
            false;           // Whether this is typically passed by reference
        bool auto_gc = true; // Whether Lua should automatically GC this
    };
    std::unordered_map<std::type_index, TypeMetadata> m_typeMetadata;

    // Setup a metatable with common metamethods (for no only __gc)
    template <typename T> void setup_metatable(lua_State* L) {
        const char* metatable_name = GetTypeName<T>();

        TypeMetadata metadata = get_type_metadata<T>();

        // Create metatable if it doesn't exist
        if (luaL_newmetatable(L, metatable_name)) {
            // Set __gc if auto_gc is true
            if (metadata.auto_gc) {
                lua_pushstring(L, "__gc");
                lua_pushcfunction(L, create_gc_function<T>());
                lua_settable(L, -3);
            }
            if (metadata.is_reference_type) {
                lua_pushstring(L, "__eq");
                lua_pushcfunction(L, [](lua_State* L) -> int {
                    T** ptr1 = static_cast<T**>(lua_touserdata(L, 1));
                    T** ptr2 = static_cast<T**>(lua_touserdata(L, 2));
                    lua_pushboolean(L, *ptr1 == *ptr2);
                    return 1;
                });
                lua_settable(L, -3);
            }
            lua_newtable(L); // Create a new table for methods
            lua_setfield(L, -2,
                         "__methods"); // Set the methods table in the metatable
            lua_newtable(L);           // Create a new table for getters
            lua_setfield(L, -2,
                         "__get"); // Set the getters table in the metatable
            lua_newtable(L);       // Create a new table for setters
            lua_setfield(L, -2,
                         "__set"); // Set the setters table in the metatable
        }

        // Pop the metatable
        lua_pop(L, 1);
    }
    // Helper to create proper __gc metamethod based on type
    template <typename T> lua_CFunction create_gc_function() {
        bool is_reference = get_type_metadata<T>().is_reference_type;
        if (is_reference) {
            // For reference types, delete the pointer
            return [](lua_State* L) -> int {
                T** ptr = static_cast<T**>(lua_touserdata(L, 1));
                if (ptr && *ptr) {
                    delete *ptr;
                    *ptr = nullptr;
                }
                return 0;
            };
        } else {
            // For value types, call the destructor explicitly
            return [](lua_State* L) -> int {
                T* obj = static_cast<T*>(lua_touserdata(L, 1));
                if (obj) {
                    obj->~T(); // Explicit destructor call
                }
                return 0;
            };
        }
    }

    template <typename T> TypeMetadata get_type_metadata() const {
        auto type_index = std::type_index(typeid(T));
        auto it = m_typeMetadata.find(type_index);
        if (it != m_typeMetadata.end()) {
            return it->second;
        } else {
            throw std::runtime_error("Type metadata not found for type " +
                                     std::string(type_index.name()));
        }
    }

  public:
    // Singleton accessor
    static LuaUtil& Get() {
        static LuaUtil instance;
        return instance;
    }

    // Register a metatable name for a C++ type with optional metadata
    template <typename T>
    void NewType(lua_State* L, const std::string& name,
                 bool is_reference_type = false, bool auto_gc = true) {
        // Put typename in the registry
        std::type_index type_idx(typeid(T));
        m_typeRegistry[type_idx] = name;

        // Store some metadata about the type
        TypeMetadata metadata;
        metadata.is_reference_type = is_reference_type;
        metadata.auto_gc = auto_gc;
        m_typeMetadata[type_idx] = metadata;

        // Register the metatable in Lua
        setup_metatable<T>(L);
    }

    // Non-template version that takes a type_index directly
    const char* GetTypeName(const std::type_index& type_index) const {
        auto it = m_typeRegistry.find(type_index);
        if (it != m_typeRegistry.end()) {
            return it->second.c_str();
        } else {
            throw std::runtime_error("Could not find metatable for type " +
                                     std::string(type_index.name()));
        }
    }

    // Get the metatable name for a C++ type
    template <typename T> const char* GetTypeName() const {
        auto type_index = std::type_index(typeid(T));
        return GetTypeName(type_index);
    }

    // Create an new instance of a registered type and push to Lua stack
    template <typename T, typename... Args>
    T* CreateAndPush(lua_State* L, Args&&... args) {
        std::type_index type_index(typeid(T));

        // Check if we have metadata for this type
        bool is_reference = get_type_metadata<T>().is_reference_type;

        // Determine if we should store the object directly or as a pointer
        T* ins = nullptr;
        if (is_reference) {
            // For reference types, store a pointer to a heap-allocated object
            T** ptr = static_cast<T**>(lua_newuserdatauv(L, sizeof(T*), 0));
            *ptr = new T(std::forward<Args>(args)...);
            ins = *ptr;
        } else {
            // For value types, allocate the object directly in Lua
            T* obj = static_cast<T*>(lua_newuserdatauv(L, sizeof(T), 0));
            new (obj) T(std::forward<Args>(args)...); // Placement new
            ins = obj;
        }

        // Set the metatable
        luaL_getmetatable(L, GetTypeName<T>());
        lua_setmetatable(L, -2);

        return ins;
    }

    // Helper to check if a userdata is of a specific type
    template <typename T> T* CheckUserdata(lua_State* L, int index) const {
        // Get metatable name for this type
        const char* metatable_name = GetTypeName<T>();

        // Check if reference type
        bool is_reference = get_type_metadata<T>().is_reference_type;
        if (is_reference) {
            // If reference type, we're storing a pointer to T
            T** ptr =
                static_cast<T**>(luaL_checkudata(L, index, metatable_name));
            return *ptr;
        } else {
            // If value type, we're storing T directly
            return static_cast<T*>(luaL_checkudata(L, index, metatable_name));
        }
    }

    // This methods wraps a pointer and pushes it to the lua stack.
    // It has to be a reference type for this to work.
    template <typename T> T* WrapAndPush(lua_State* L, T* ptr) {
        bool is_reference = get_type_metadata<T>().is_reference_type;
        if (!is_reference) {
            std::cerr << "type is not a reference type!" << std::endl;
            throw std::runtime_error(
                "Type is not a reference type, cannot wrap it.");
        }

        // Create a new userdata and store the pointer
        T** userdata = static_cast<T**>(lua_newuserdatauv(L, sizeof(T*), 0));
        *userdata = ptr;
        // Set the metatable
        luaL_getmetatable(L, GetTypeName<T>());
        lua_setmetatable(L, -2);
        return *userdata;
    }

    void PushLibraryMetatable(lua_State* L) {
        const char* library_metatable_name = "Library";
        if (!luaL_getmetatable(L, library_metatable_name)) {
            lua_pop(L, 1);
            luaL_newmetatable(L, library_metatable_name);
            lua_pushvalue(L, -2); // Push the metatable itself
            lua_setfield(L, -2,
                         "__index"); // Set __index to the metatable itself
            lua_pushcfunction(L, [](lua_State* L) -> int {
                // First argument is the table itself
                luaL_checktype(L, 1, LUA_TTABLE);
                lua_pushvalue(L, 1); // Push the table to the stack
                lua_getfield(L, -1, "new");
                auto constructor = lua_tocfunction(L, -1);

                return constructor(L); // Call the constructor
            });
            lua_setfield(L, -2, "__call");
        }
    }

    // Delete copy/move constructors and assignment operators
    LuaUtil(const LuaUtil&) = delete;
    LuaUtil& operator=(const LuaUtil&) = delete;
    LuaUtil(LuaUtil&&) = delete;
    LuaUtil& operator=(LuaUtil&&) = delete;
};