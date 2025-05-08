#include "LuaPrimitive.hpp"
#include "LuaType.hpp"
#include "LuaVector3.hpp"

#include "Primitive.hpp"
#include "SceneManager.hpp"
#include "Enums.hpp"
#include <lua.hpp>
#include <iostream>
#include <glm/glm.hpp>

LuaPrimitive::LuaPrimitive(PrimitiveType type) {
    this->m_ref =
        SceneManager::Get().AddEntity(type, RigidBodyType::Dynamic, 0);
}

LuaPrimitive::LuaPrimitive(PrimitiveType type, LuaMaterial* material) {
    this->m_ref = SceneManager::Get().AddEntity(type, RigidBodyType::Dynamic,
                                                material->GetID());
}

LuaPrimitive::~LuaPrimitive() {
    // Destructor logic if needed
    std::cout << "LuaPrimitive destructor called for primitive" << std::endl;
}

void LuaPrimitive::SetPosition(LuaVector3& position) {
    m_ref.data->SetPhysicsPosition(position.Get());
}
LuaVector3 LuaPrimitive::GetPosition() {
    return LuaVector3(m_ref.data->GetPosition());
}

PrimitiveType LuaPrimitive::GetType() {
    Primitive* p = dynamic_cast<Primitive*>(m_ref.data);
    return p->GetType();
}
void LuaPrimitive::SetType(PrimitiveType type) {
    SceneManager::Get().UpdateEntity(m_ref.id, type);
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
    LuaPrimitive* self = LuaUtil::Get().CheckUserdata<LuaPrimitive>(L, 1);
    PrimitiveType type = self->GetType();
    int typeInt = PrimitiveTypeToInt(type);
    lua_pushinteger(L, typeInt);
    return 1;
}

int LuaPrimitive::luaSetType(lua_State* L) {
    LuaPrimitive* self = LuaUtil::Get().CheckUserdata<LuaPrimitive>(L, 1);
    int typeInt = luaL_checkinteger(L, 2);
    PrimitiveType type = IntToPrimitiveType(typeInt);
    self->SetType(type);
    return 0;
}

// Returns a copy of the position vector
int LuaPrimitive::luaGetPosition(lua_State* L) {
    LuaPrimitive* self = LuaUtil::Get().CheckUserdata<LuaPrimitive>(L, 1);
    LuaVector3 pos = self->GetPosition();
    LuaUtil::Get().CreateAndPush<LuaVector3>(L, pos.GetX(), pos.GetY(),
                                             pos.GetZ());
    return 1;
}

int LuaPrimitive::luaSetPosition(lua_State* L) {
    LuaPrimitive* self = LuaUtil::Get().CheckUserdata<LuaPrimitive>(L, 1);
    LuaVector3* position = LuaUtil::Get().CheckUserdata<LuaVector3>(L, 2);

    if (position) {
        self->SetPosition(*position);
    } else {
        std::cerr << "Invalid position vector" << std::endl;
        return luaL_error(L, "Invalid position vector");
    }
    return 0;
}

int LuaPrimitive::luaDestroy(lua_State* L) {
    LuaPrimitive* self = LuaUtil::Get().CheckUserdata<LuaPrimitive>(L, 1);
    SceneManager::Get().RemoveEntity(self->m_ref.id);
    return 0;
}

int LuaPrimitive::luaNew(lua_State* L) {
    // Get possible argument that is an int
    int type = 0;
    if (lua_isnumber(L, 1)) {
        type = luaL_checkinteger(L, 1);
        std::cout << "LuaPrimitive type: " << type << std::endl;
    }
    // LuaMaterial* material = LuaUtil::Get().CheckUserdata<LuaMaterial>(L, 2);
    // if (material) {
    //     LuaUtil::Get().CreateAndPush<LuaPrimitive>(L, IntToPrimitiveType(type), material);
    //     return 1;
    // }
    LuaUtil::Get().CreateAndPush<LuaPrimitive>(L, IntToPrimitiveType(type));
    return 1;
}
