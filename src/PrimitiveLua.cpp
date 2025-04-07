#include "PrimitiveLua.hpp"
#include "lua.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <string>

PrimitiveLua::PrimitiveLua(const std::string& name) : name(name) {
    // Initialize position to (0, 0, 0) and color to white (0xffffffff)
    position = glm::vec3(1.0f, 2.0f, 3.0f);
    color = 0xffffffff;
}

PrimitiveLua::PrimitiveLua()
    : name(""), position(1.0f, 2.0f, 3.0f), color(0xffffffff) {}

PrimitiveLua::~PrimitiveLua() {
    // Destructor logic if needed
    std::cout << "PrimitiveLua destructor called for primitive" << std::endl;
}

std::string& PrimitiveLua::GetName() { return name; }

// void PrimitiveLua::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
//     color = (r << 24) | (g << 16) | (b << 8) | a;
// }

// const uint32_t& PrimitiveLua::GetColor() const {
//     return color;
// }

void PrimitiveLua::SetPosition(glm::vec3& position) {
    this->position = position;
}

glm::vec3& PrimitiveLua::GetPosition() { return position; }

namespace {
int luaSetPosition(lua_State* L) {
    PrimitiveLua* i = reinterpret_cast<PrimitiveLua*>(
        luaL_checkudata(L, 1,
                        PrimitiveLua::metatableName
                            .c_str())); // Check if the first argument is a
                                        // userdata of type PrimitiveLua
    int nargs = lua_gettop(L);          // Get the number of arguments
    luaL_argcheck(L, i != nullptr, 1, "'Primitive' expected");
    luaL_argcheck(L, i != nullptr, 2, "'Number' expected");
    luaL_argcheck(L, i != nullptr, 3, "'Number' expected");
    luaL_argcheck(L, i != nullptr, 4, "'Number' expected");

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int z = luaL_checkinteger(L, 4);
    glm::vec3 position = glm::vec3(x, y, z);
    i->SetPosition(position);

    return 0;
}
} // namespace

#include <iostream>
#include <lua.hpp>

void printLuaStack(lua_State* L) {
    int top = lua_gettop(L);
    std::cout << "Lua stack (top -> bottom):\n";

    for (int i = top; i >= 1; --i) {
        int type = lua_type(L, i);
        std::cout << i << ": " << lua_typename(L, type) << " - ";

        switch (type) {
        case LUA_TSTRING:
            std::cout << lua_tostring(L, i);
            break;
        case LUA_TBOOLEAN:
            std::cout << (lua_toboolean(L, i) ? "true" : "false");
            break;
        case LUA_TNUMBER:
            std::cout << lua_tonumber(L, i);
            break;
        case LUA_TNIL:
            std::cout << "nil";
            break;
        default:
            std::cout << lua_topointer(L, i); // Print pointer for other types
            break;
        }
        std::cout << "\n";
    }
}

namespace {
int _luaNewIndexPrimitive(lua_State* L) {
    // 1st arg userdata, 2nd arg key, 3rd arg value
    PrimitiveLua* i = reinterpret_cast<PrimitiveLua*>(luaL_checkudata(
        L, 1,
        PrimitiveLua::metatableName.c_str())); // Check if the first argument is
                                               // userdata of type PrimitiveLua
    luaL_argcheck(L, i != nullptr, 1,
                  ("'" + PrimitiveLua::luaName + "'expected").c_str());
    std::string key = luaL_checkstring(L, 2);
    if (key == "Position") {
        luaL_checktype(L, 3,
                       LUA_TTABLE); // Like argcheck, but only for the type
        lua_getfield(L, 3, "x");
        lua_getfield(L, 3, "y");
        lua_getfield(L, 3, "z");
        lua_remove(L, 2);
        lua_remove(L, 2);
        return luaSetPosition(L);
    }

    return luaL_error(L, ("No property " + key + " on primitive").c_str());
}
} // namespace

namespace {
int luaGetName(lua_State* L) {
    PrimitiveLua* i = (PrimitiveLua*)luaL_checkudata(
        L, 1,
        PrimitiveLua::metatableName.c_str()); // Check if the first argument is
                                              // a userdata of type PrimitiveLua

    luaL_argcheck(L, i != nullptr, 1, "'Primitive' expected");
    std::string& name = i->GetName();
    lua_pushstring(L, name.c_str()); // Push the name onto the stack
    return 1;
}
} // namespace

namespace {
int luaGetPosition(lua_State* L) {
    PrimitiveLua* i = reinterpret_cast<PrimitiveLua*>(
        luaL_checkudata(L, 1,
                        PrimitiveLua::metatableName
                            .c_str())); // Check if the first argument is a
    // userdata of type PrimitiveLua
    luaL_argcheck(L, i != nullptr, 1, "'Primitive' expected");

    glm::vec3& pos = i->GetPosition();
    // std::cout << i->GetName() << std::endl;
    // std::cout << i->GetPosition().x << std::endl;
    // std::cout << i->GetPosition().y << std::endl;
    // std::cout << i->GetPosition().z << std::endl;

    lua_newtable(L); // Create a new table to hold the position
    lua_pushnumber(L, pos.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, pos.y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, pos.z);
    lua_setfield(L, -2, "z");
    return 1;
}
} // namespace

namespace {
int newPrimitive(lua_State* L) {
    PrimitiveLua* userdata =
        (PrimitiveLua*)lua_newuserdatauv(L, sizeof(PrimitiveLua), 0);
    new (userdata) PrimitiveLua("New Primitive");
    luaL_setmetatable(L, PrimitiveLua::metatableName.c_str());
    return 1;
}
} // namespace

namespace {
int _luaIndexPrimitive(lua_State* L) {
    PrimitiveLua* i = (PrimitiveLua*)luaL_checkudata(
        L, 1, PrimitiveLua::metatableName.c_str());

    luaL_argcheck(L, i != nullptr, 1, "'Primitive' expected");
    std::string key = luaL_checkstring(L, 2);
    //std::cout << "key: " << key << std::endl;

    if (key == "Position") {
        return luaGetPosition(L);
    }
    if (key == "Name") {
        return luaGetName(L);
    }

    // TODO: FIX METHODS
    // Find the method in the the metatable
    lua_getmetatable(L, 1); // Push the metatable onto the stack
    if (LUA_TFUNCTION != lua_getfield(L, -1, key.c_str())) {
        lua_remove(L, -1);
        lua_remove(L, -1);
        lua_pushnil(L);
        return 1; // Return nil if the method is not found
    }; // Push the method onto the stack
    lua_remove(L, -2);
    return 1;
}
} // namespace

const std::string PrimitiveLua::luaName = "Primitive";
const std::string PrimitiveLua::metatableName = "Primitive.Metatable";
const luaL_Reg PrimitiveLua::methods[] = {
    {"SetPosition", &luaSetPosition},
    {"GetPosition", &luaGetPosition},
    {"GetName", &luaGetName},
    {nullptr, nullptr}}; // End of the methods array
const luaL_Reg PrimitiveLua::functions[] = {
    {"new", &newPrimitive}, {nullptr, nullptr}}; // End of the functions array
const lua_CFunction PrimitiveLua::luaIndexPrimitive = &_luaIndexPrimitive;
const lua_CFunction PrimitiveLua::luaNewIndexPrimitive = &_luaNewIndexPrimitive;