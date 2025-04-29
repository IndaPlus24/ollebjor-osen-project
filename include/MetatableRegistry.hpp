// #include <unordered_map>
#include <iostream>
#include <typeindex>
#include <string>
#include <stdexcept>
#include <functional>
#include <lua.hpp>

class MetatableRegistry {
private:
    // Private constructor prevents external instantiation
    MetatableRegistry() {}

    // Store mapping between C++ types and Lua metatable names
    std::unordered_map<std::type_index, std::string> metatable_registry;
    
    // Store constructor factories for each type
    std::unordered_map<std::type_index, std::function<int(lua_State*)>> constructor_factories;
    
    // Store additional type metadata (optional)
    struct TypeMetadata {
        bool is_reference_type = false;  // Whether this is typically passed by reference
        bool auto_gc = true;             // Whether Lua should automatically GC this
        size_t sizeof_object = 0;        // Size of the object
    };
    std::unordered_map<std::type_index, TypeMetadata> type_metadata;

public:
    // Singleton accessor
    static MetatableRegistry& Get() {
        static MetatableRegistry registry;
        return registry;
    }
    
    // Register a metatable name for a C++ type with optional metadata
    template <typename T>
    void register_metatable(const std::string& name, bool is_reference_type = false, bool auto_gc = true) {
        std::type_index type_idx(typeid(T));
        metatable_registry[type_idx] = name;
        
        // Store some metadata about the type
        TypeMetadata metadata;
        metadata.is_reference_type = is_reference_type;
        metadata.auto_gc = auto_gc;
        metadata.sizeof_object = sizeof(T);
        type_metadata[type_idx] = metadata;
    }
    
    // Register a constructor factory for a type
    template <typename T>
    void register_constructor(std::function<int(lua_State*)> constructor_factory) {
        constructor_factories[std::type_index(typeid(T))] = constructor_factory;
    }

    // Non-template version that takes a type_index directly
    const char* get_metatable_name(const std::type_index& type) const {
        auto it = metatable_registry.find(type);
        if (it != metatable_registry.end()) {
            return it->second.c_str();
        } else {
            throw std::runtime_error("Metatable not registered for type " + std::string(type.name()));
        }
    }
    
    // Get the metatable name for a C++ type
    template <typename T>
    const char* get_metatable_name() const {
       auto type_index = std::type_index(typeid(T));
        return get_metatable_name(type_index);
    }
    
    // Create an instance of a registered type and push to Lua stack
    template <typename T, typename... Args>
    T* create_and_push(lua_State* L, Args&&... args) {
        std::type_index type_idx(typeid(T));
        
        // Check if we have metadata for this type
        auto meta_it = type_metadata.find(type_idx);
        if (meta_it == type_metadata.end()) {
            return nullptr;
        }
        
        // Determine if we should store the object directly or as a pointer
        T* ins = nullptr;
        if (meta_it->second.is_reference_type) {
            // For reference types, store a pointer to a heap-allocated object
            T** ptr = static_cast<T**>(lua_newuserdata(L, sizeof(T*)));
            *ptr = new T(std::forward<Args>(args)...);
            ins = *ptr;
        } else {
            // For value types, allocate the object directly in Lua
            T* obj = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
            new (obj) T(std::forward<Args>(args)...);  // Placement new
            ins = obj;
        }
        
        // Set the metatable
        luaL_getmetatable(L, get_metatable_name<T>());
        lua_setmetatable(L, -2);
        
        return ins;
    }
    
    // Helper to check if a userdata is of a specific type
    template <typename T>
    T* check_userdata(lua_State* L, int index) const {
        // Get metatable name for this type
        const char* metatable_name = get_metatable_name<T>();
        
        // Check if reference type
        auto meta_it = type_metadata.find(std::type_index(typeid(T)));
        bool is_reference = meta_it != type_metadata.end() && meta_it->second.is_reference_type;
        
        if (is_reference) {
            // If reference type, we're storing a pointer to T
            T** ptr = static_cast<T**>(luaL_checkudata(L, index, metatable_name));
            return *ptr;
        } else {
            // If value type, we're storing T directly
            return static_cast<T*>(luaL_checkudata(L, index, metatable_name));
        }
    }
    
    // Helper to create proper __gc metamethod based on type
    template <typename T>
    lua_CFunction create_gc_function() {
        auto meta_it = type_metadata.find(std::type_index(typeid(T)));
        bool is_reference = meta_it != type_metadata.end() && meta_it->second.is_reference_type;
        
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
                    std::cout << "Calling destructor for object of type " << typeid(T).name() << std::endl;
                    //FIXME: this might cause error
                    obj->~T();  // Explicit destructor call
                }
                return 0;
            };
        }
    }
    
    // Setup a metatable with common metamethods
    template <typename T>
    void setup_metatable(lua_State* L) {
        const char* name = get_metatable_name<T>();
        
        // Create metatable if it doesn't exist
        if (luaL_newmetatable(L, name)) {

            // Set __gc if auto_gc is true
            auto meta_it = type_metadata.find(std::type_index(typeid(T)));
            if (meta_it != type_metadata.end() && meta_it->second.auto_gc) {
                lua_pushstring(L, "__gc");
                lua_pushcfunction(L, create_gc_function<T>());
                lua_settable(L, -3);
            }
        }
        
        // Pop the metatable
        lua_pop(L, 1);
    }

    template<typename T>
    TypeMetadata get_type_metadata() const {
        auto type = std::type_index(typeid(T));
        auto it = type_metadata.find(type);
        if (it != type_metadata.end()) {
            return it->second;
        } else {
            throw std::runtime_error("Type metadata not found for type " + std::string(type.name()));
        }
    }

    // Delete copy/move constructors and assignment operators
    MetatableRegistry(const MetatableRegistry&) = delete;
    MetatableRegistry& operator=(const MetatableRegistry&) = delete;
    MetatableRegistry(MetatableRegistry&&) = delete;
    MetatableRegistry& operator=(MetatableRegistry&&) = delete;
};