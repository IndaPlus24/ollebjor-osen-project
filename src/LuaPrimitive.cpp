#include "LuaExporter.hpp"

#include "LuaPrimitive.hpp"
#include "LuaVector3.hpp"

#include "Enums.hpp"
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
    LuaPrimitive* self =
        MetatableRegistry::instance().check_userdata<LuaPrimitive>(L, 1);
    PrimitiveType type = self->GetType();
    int typeInt = PrimitiveTypeToInt(type);
    lua_pushinteger(L, typeInt);
    return 1;
}

int LuaPrimitive::luaSetType(lua_State* L) {
    LuaPrimitive* self =
        MetatableRegistry::instance().check_userdata<LuaPrimitive>(L, 1);
    int typeInt = luaL_checkinteger(L, 2);
    PrimitiveType type = IntToPrimitiveType(typeInt);
    self->SetType(type);
    return 0;
}

// Returns a copy of the position vector
int LuaPrimitive::luaGetPosition(lua_State* L) {
    LuaPrimitive* self =
        MetatableRegistry::instance().check_userdata<LuaPrimitive>(L, 1);
    LuaVector3& pos = self->GetPosition();
    MetatableRegistry::instance().create_and_push<LuaVector3>(
        L, pos.GetX(), pos.GetY(), pos.GetZ());
    return 1;
}

int LuaPrimitive::luaSetPosition(lua_State* L) {
    LuaPrimitive* self =
        MetatableRegistry::instance().check_userdata<LuaPrimitive>(L, 1);
    LuaVector3* position =
        MetatableRegistry::instance().check_userdata<LuaVector3>(L, 2);

    if (position) {
        self->SetPosition(*position);
    } else {
        std::cerr << "Invalid position vector" << std::endl;
        return luaL_error(L, "Invalid position vector");
    }
    return 0;
}

int LuaPrimitive::luaNew(lua_State* L) {
    //TODO: make it so that it can set the type
    //TODO: Tell Scene manager to create a primitive and recieve the id and pointer to that
    MetatableRegistry::instance().create_and_push<LuaPrimitive>(L, PrimitiveType::Cube);
    return 1;
}