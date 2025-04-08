#include "LuaPrimitive.hpp"
#include "Enums.hpp"
#include "LuaVector3.hpp"
#include <lua.hpp>
#include <iostream>
#include <glm/glm.hpp>

LuaPrimitive::LuaPrimitive(PrimitiveType type) {
    // Initialize position to (0, 0, 0) and color to white (0xffffffff)
    this->position = LuaVector3(0.0f, 0.0f, 0.0f);
    this->type = type;
    // color = 0xffffffff;
}

LuaPrimitive::LuaPrimitive(PrimitiveType type, LuaVector3& position) {
    // Initialize position to (0, 0, 0) and color to white (0xffffffff)
    this->position = LuaVector3(0.0f, 0.0f, 0.0f);
    this->type = type;
    // color = 0xffffffff;
}

LuaPrimitive::~LuaPrimitive() {
    // Destructor logic if needed
    std::cout << "LuaPrimitive destructor called for primitive" << std::endl;
}

// void LuaPrimitive::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
//     color = (r << 24) | (g << 16) | (b << 8) | a;
// }

// const uint32_t& LuaPrimitive::GetColor() const {
//     return color;
// }

void LuaPrimitive::SetPosition(LuaVector3& position) {
    this->position = position;
}
LuaVector3& LuaPrimitive::GetPosition() { return position; }

PrimitiveType LuaPrimitive::GetType() { return type; }
void LuaPrimitive::SetType(PrimitiveType type) { this->type = type; }

int PrimitiveTypeToInt(PrimitiveType type) {
    switch (type) {
    case PrimitiveType::Cube:
        return 0;
    case PrimitiveType::Sphere:
        return 1;
    case PrimitiveType::Plane:
        return 2;
    default:
        return -1; // Invalid type
    }
}
PrimitiveType IntToPrimitiveType(int type) {
    switch (type) {
    case 0:
        return PrimitiveType::Cube;
    case 1:
        return PrimitiveType::Sphere;
    case 2:
        return PrimitiveType::Plane;
    default:
        return PrimitiveType::Cube; // Invalid type
    }
}

int LuaPrimitive::luaGetType(lua_State* L) {
    const char* mtName = lua_tostring(L, lua_upvalueindex(1));
    LuaPrimitive* primitive = (LuaPrimitive*)(luaL_checkudata(L, 1, mtName));

    PrimitiveType type = primitive->GetType();
    int typeInt = PrimitiveTypeToInt(type);
    lua_pushinteger(L, typeInt);
    return 1;
}

int LuaPrimitive::luaSetType(lua_State* L) {
    const char* mtName = lua_tostring(L, lua_upvalueindex(1));
    LuaPrimitive* primitive = (LuaPrimitive*)(luaL_checkudata(L, 1, mtName));

    int typeInt = luaL_checkinteger(L, 2);
    PrimitiveType type = IntToPrimitiveType(typeInt);
    primitive->SetType(type);
    return 0;
}

// Returns a copy of the position vector
int LuaPrimitive::luaGetPosition(lua_State* L) {
    const char* mtName = lua_tostring(L, lua_upvalueindex(1));
    LuaPrimitive* primitive = (LuaPrimitive*)(luaL_checkudata(L, 1, mtName));
    LuaVector3& pos = primitive->GetPosition();
    lua_pop(L, 1);
    lua_pushnumber(L, pos.GetX());
    lua_pushnumber(L, pos.GetY());
    lua_pushnumber(L, pos.GetZ());
    return LuaVector3::luaNewVector3(L);
}

int LuaPrimitive::luaSetPosition(lua_State* L) {
    const char* mtName = lua_tostring(L, lua_upvalueindex(1));
    LuaPrimitive* primitive = (LuaPrimitive*)(luaL_checkudata(L, 1, mtName));
    LuaVector3* position = (LuaVector3*)(luaL_checkudata(
        L, 2, "Vector3.Metatable")); // FIXME: this should not be hardcoded
    primitive->SetPosition(*position);
    return 0;
}

int LuaPrimitive::luaNew(lua_State* L) {
    const char* mtName = lua_tostring(L, lua_upvalueindex(1));
    LuaPrimitive* primitive =
        (LuaPrimitive*)lua_newuserdatauv(L, sizeof(LuaPrimitive), 0);
    // TODO: Fix this so that it it has optional parameters, and the type is not
    // hardcoded
    new (primitive) LuaPrimitive(PrimitiveType::Cube); // Default to Cube type
    luaL_setmetatable(L, mtName);
    return 1;
}