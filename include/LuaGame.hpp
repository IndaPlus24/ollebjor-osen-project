#pragma once
#include <string>
#include <lua.hpp>
#include <typeindex>
#include "LuaExporter.hpp"
#include "LuaService.hpp"
#include <unordered_map>

class LuaGame {

  public:
    template <typename T> T& GetService() const;
    template <typename T> T& GetService(std::string service) const;
    template <typename T, typename... Args>
    LuaExporter<T> AddService(lua_State* L, std::string service, Args... args);
    static int luaGet(lua_State* L);

  private:
    std::unordered_map<std::type_index, LuaService> m_services;
    std::unordered_map<std::string, std::type_index> m_serviceNames;
};