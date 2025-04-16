#include "LuaExporter.hpp"

#include "LuaPrimitive.hpp"
#include "LuaVector3.hpp"

#include "Primitive.hpp"
#include "SceneManager.hpp"
#include "Enums.hpp"
#include <lua.hpp>
#include <iostream>
#include <glm/glm.hpp>

LuaPrimitive::LuaPrimitive(PrimitiveType type) {
    std::cout << "LuaPrimitive constructor called for primitive" << std::endl;
    this->m_ref =
        SceneManager::GetInstance().AddEntity(type, RigidBodyType::Dynamic, 0);
    std::cout << "LuaPrimitive created" << std::endl;
}

LuaPrimitive::LuaPrimitive(PrimitiveType type, LuaVector3& position) {
    this->m_ref = SceneManager::GetInstance().AddEntity(
        type, RigidBodyType::Dynamic, 0, position.Get());
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
    m_ref.data->SetPhysicsPosition(position.Get());
}
LuaVector3 LuaPrimitive::GetPosition() {
    return LuaVector3(m_ref.data->GetPosition());
}

PrimitiveType LuaPrimitive::GetType() {
    Primitive* p = static_cast<Primitive*>(m_ref.data);
    return p->GetType();
}
void LuaPrimitive::SetType(PrimitiveType type) {
    Primitive* p = static_cast<Primitive*>(m_ref.data);
    p->SetType(type);
}

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
    LuaVector3 pos = self->GetPosition();
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
    MetatableRegistry::instance().create_and_push<LuaPrimitive>(
        L, PrimitiveType::Sphere);
    std::cout << "LuaPrimitive created" << std::endl;
    return 1;
}