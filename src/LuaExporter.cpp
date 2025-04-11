// #include "LuaExporter.hpp"
// #include <iostream>
// #include <lua.hpp>
// #include <string>
// #include <vector>
// #include "MetatableRegistry.hpp"
// #include "lua.h"

// template <typename T>
// LuaExporter<T>::LuaExporter(lua_State* L, std::string luaName,
//                             bool isReferenceType, bool autoGC) {
//     MetatableRegistry::instance().register_metatable<T>(
//         luaName + ".Metatable", isReferenceType, autoGC);
//     this->name = luaName;
//     this->L = L;
// }

// template <typename T> LuaExporter<T>::~LuaExporter() {
//     // Destructor logic if needed
// }

// template <typename T>
// LuaExporter<T> LuaExporter<T>::Func(std::string name, lua_CFunction func,
//                                     int nargs) {
//     luaL_Reg funcReg = {name.c_str(), func};
//     funcs.push_back(funcReg);
//     return *this;
// }

// template <typename T> LuaExporter<T> LuaExporter<T>::Funcs(luaL_Reg funcs[]) {
//     for (int i = 0; funcs[i].name != nullptr; i++) {
//         this->funcs.push_back(funcs[i]);
//     }
//     return *this;
// }

// template <typename T>
// LuaExporter<T> LuaExporter<T>::Method(std::string name, lua_CFunction func,
//                                       int nargs) {
//     luaL_Reg method = {name.c_str(), func};
//     methods.push_back(method);
//     return *this;
// }

// template <typename T>
// LuaExporter<T> LuaExporter<T>::Methods(luaL_Reg methods[]) {
//     for (int i = 0; methods[i].name != nullptr; i++) {
//         this->methods.push_back(methods[i]);
//     }
//     return *this;
// }

// template <typename T>
// LuaExporter<T> LuaExporter<T>::Setter(std::string name,
//                                         lua_CFunction setter) {
//     luaL_Reg prop = {name.c_str(), setter};
//     metamethods.push_back(prop);
//     return *this;
// }

// template <typename T>
// LuaExporter<T> LuaExporter<T>::Getter(std::string name, lua_CFunction getter) {
//     luaL_Reg prop = {name.c_str(), getter};
//     getters.push_back(prop);
//     return *this;
// }

// template <typename T>
// LuaExporter<T> LuaExporter<T>::Constructor(lua_CFunction constFac, int nargs) {
//     luaL_Reg func = {"new", constFac};
//     funcs.push_back(func);
//     MetatableRegistry::instance().register_constructor<T>(constFac);
//     return *this;
// }

// template <typename T> lua_CFunction __index_wrapper() {

//     return [](lua_State* L) -> int {
//         const char* mtName =
//             MetatableRegistry::instance().get_metatable_name<T>();
//         void* obj = (void*)luaL_checkudata(L, 1, mtName);

//         const char* key = luaL_checkstring(L, 2);
//         int type = lua_type(L, 2);
//         lua_pop(L, 1); // Pop the key off the stack

//         if (type == LUA_TSTRING) {
//             // std::cout << "It is a string: " << key << std::endl;
//         } else {
//             return luaL_error(L,
//                               "Invalid key type, only strings are supported. "
//                               "This might change in the future.");
//         }

//         luaL_getmetatable(L, mtName);
//         lua_getfield(L, -1, "__get");
//         lua_getfield(L, -1, key);

//         // First check __get
//         if (!lua_isnil(L, -1)) {
//             // We have the getter function on the top of the stack
//             lua_remove(L, -2);
//             lua_remove(L, -2);
//             lua_insert(L, -2);
//             int status = lua_pcall(L, 1, 1, 0);
//             if (status != LUA_OK) {
//                 const char* error = lua_tostring(L, -1);
//                 std::cerr << "Error in __index: " << error << std::endl;
//                 lua_pop(L, 1); // Pop the error message off the stack
//                 return 0;
//             }
//             return 1;
//         }

//         lua_pop(L, 2); // Pop the nil value and __get of the stack
//         // Then check __methods
//         lua_getfield(L, -1, "__methods");
//         lua_getfield(L, -1, key);
//         return 1;
//     };
// }

// template <typename T> lua_CFunction __newindex_wrapper() {
//     return [](lua_State* L) -> int {
//         const char* mtName =
//             MetatableRegistry::instance().get_metatable_name<T>();
//         void* obj = (void*)luaL_checkudata(L, 1, mtName);

//         const char* key = luaL_checkstring(L, 2);
//         int type = lua_type(L, 2);
//         lua_pop(L, 1); // Pop the key off the stack

//         if (type == LUA_TSTRING) {
//             // std::cout << "It is a string: " << key << std::endl;
//         } else {
//             return luaL_error(L,
//                               "Invalid key type, only strings are supported. "
//                               "This might change in the future.");
//         }

//         lua_getmetatable(L, 1);
//         lua_getfield(L, -1, "__set");
//         lua_getfield(L, -1, key);

//         // First check __set
//         if (!lua_isnil(L, -1)) {
//             // We have the setter function on the top of the stack
//             lua_remove(L, -2);
//             lua_remove(L, -2);
//             lua_insert(L, -2);
//             int status = lua_pcall(L, 1, 1, 0);
//             if (status != LUA_OK) {
//                 const char* error = lua_tostring(L, -1);
//                 std::cerr << "Error in __newindex: " << error << std::endl;
//                 lua_pop(L, 1); // Pop the error message off the stack
//             }
//             return 0;
//         }
//         return 1;
//     };
// }

// /// Check if the field exists in the table on the top of the stack
// /// @param L The Lua state
// /// @param field The field name to check
// /// @return True if the field exists, false otherwise
// bool has_field(lua_State* L, const char* field) {
//     lua_getfield(L, -1, field);
//     bool hasField = !lua_isnil(L, -1);
//     lua_pop(L, 1);
//     return hasField;
// }

// template <typename T> void LuaExporter<T>::Export() {
//     funcs.push_back({nullptr, nullptr});       // End of the functions array
//     methods.push_back({nullptr, nullptr});     // End of the methods array
//     metamethods.push_back({nullptr, nullptr}); // End of the metamethods array
//     getters.push_back({nullptr, nullptr});     // End of the getters array

//     MetatableRegistry::instance().setup_metatable<T>(
//         L); // Creates a default metatable with __name and __gc
//     const char* mtName = MetatableRegistry::instance().get_metatable_name<T>();

//     // metatable.__methods = methods
//     lua_newtable(L);
//     luaL_setfuncs(L, methods.data(), 0);
//     lua_setfield(L, -2, "__methods");

//     // metatable.__get = getters
//     lua_newtable(L);
//     luaL_setfuncs(L, getters.data(), 0);
//     lua_setfield(L, -2, "__get");

//     // metatable.__set = setters
//     lua_newtable(L);
//     luaL_setfuncs(L, getters.data(), 0);
//     lua_setfield(L, -2, "__set");

//     // Set normal metamethods directly to the metatable
//     luaL_setfuncs(L, metamethods.data(), 0);

//     // Sets the __index and __newindex metamethods to the metatable
//     // if they are not already, manually set
//     if (!has_field(L, "__index")) {
//         lua_pushcclosure(L, __index_wrapper<T>(), 0);
//         lua_setfield(L, -2, "__index");
//     }

//     if (!has_field(L, "__newindex")) {
//         lua_pushcclosure(L, __newindex_wrapper<T>(), 0);
//         lua_setfield(L, -2, "__newindex");
//     }

//     lua_createtable(L, 0, 1); // Create library table
//     lua_pushstring(L,
//                    mtName); // Pushes the name of the metatable
//     luaL_setfuncs(L, funcs.data(),
//                   1); // Register all methods in the array to the table
//     // TODO: set metatable for library table so it cannot be overwritten

//     lua_setglobal(
//         L, name.c_str()); // Consume the top of the stack and make it a global
// }

// // Add explicit template instantiations for types you use
// #include "LuaVector3.hpp" // Include the header with LuaVector3 definition
// template class LuaExporter<LuaVector3>;