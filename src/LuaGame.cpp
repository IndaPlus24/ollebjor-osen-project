#include "LuaGame.hpp"
#include "LuaExporter.hpp"
#include "LuaService.hpp"

template <typename T> T& LuaGame::GetService() const {
    auto typeIndex = std::type_index(typeid(T));
    auto it = m_services.find(typeIndex);
    if (it != m_services.end()) {
        return it->second;
    } else {
        throw std::runtime_error("Service not found");
    }
}

template <typename T> T& LuaGame::GetService(std::string service) const {
    auto typeIter = m_serviceNames.find(service);
    if (typeIter == m_serviceNames.end()) {
        throw std::runtime_error("Service not found");
    }

    auto typeIndex = typeIter->second;
    auto nameIter = m_services.find(typeIndex);

    if (nameIter == m_services.end()) {
        throw std::runtime_error("Service not found");
    }
    return nameIter->second;
}

template <typename T, typename... Args>
LuaExporter<T> LuaGame::AddService(lua_State* L, std::string luaName,
                                   Args... args) {

    // Create the luaexporter for the service
    LuaExporter<T> exporter(L, luaName);

    // Create a LuaService wrapper
    LuaService luaService;

    // Store the service using type_index as key
    auto typeIndex = std::type_index(typeid(T));
    m_services[typeIndex] = luaService;
    m_serviceNames[luaName] = typeIndex;

    return exporter;
}

int LuaGame::luaGet(lua_State* L) { return 1; }

template <typename T> void _luaGet(lua_State* L) {
    LuaGame* self = MetatableRegistry::instance().check_userdata<LuaGame>(L, 1);
    const char* service = luaL_checkstring(L, 2);
    T& serviceInstance = self->GetService<T>(service);

    return MetatableRegistry::instance().create_and_push<T>(L, serviceInstance);
}
