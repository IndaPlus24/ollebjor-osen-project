#include "LuaVector3.hpp"
#include "LuaType.hpp"

LuaVector3::LuaVector3() : position(0.0f, 0.0f, 0.0f) {}
LuaVector3::LuaVector3(glm::vec3 position) : position(position) {}
LuaVector3::LuaVector3(float x, float y, float z) : position(x, y, z) {}

LuaVector3::~LuaVector3() {
    // Destructor logic if needed
    // std::cout << "LuaVector3 destructor called for vector" << std::endl;
}

namespace {
template <typename T> T* check(lua_State* L, int index) {
    T* obj = LuaUtil::Get().CheckUserdata<T>(L, index);
    if (!obj) {
        luaL_error(L, "Invalid userdata for type %s",
                   LuaUtil::Get().GetTypeName<T>());
    }
    return obj;
}
} // namespace

float LuaVector3::GetX() const { return position.x; }
float LuaVector3::GetY() const { return position.y; }
float LuaVector3::GetZ() const { return position.z; }

float LuaVector3::GetLength() const { return glm::length(position); }
float LuaVector3::Dot(const LuaVector3& other) const {
    return glm::dot(position, other.position);
}
LuaVector3 LuaVector3::Cross(const LuaVector3& other) const {
    return LuaVector3(glm::cross(position, other.position));
}

LuaVector3 LuaVector3::Normalize() const {
    return LuaVector3(glm::normalize(position));
}

glm::vec3 LuaVector3::Get() const { return position; }
void LuaVector3::Set(const glm::vec3& vec) { position = vec; }

int LuaVector3::luaGetX(lua_State* L) {
    LuaVector3* vec = check<LuaVector3>(L, 1);
    lua_pushnumber(L, vec->GetX());
    return 1;
}

int LuaVector3::luaGetY(lua_State* L) {
    LuaVector3* vec = check<LuaVector3>(L, 1);
    lua_pushnumber(L, vec->GetY());
    return 1;
}

int LuaVector3::luaGetZ(lua_State* L) {
    LuaVector3* vec = check<LuaVector3>(L, 1);
    lua_pushnumber(L, vec->GetZ());
    return 1;
}

int LuaVector3::luaDot(lua_State* L) {
    LuaVector3* self = check<LuaVector3>(L, 1);
    LuaVector3* other = check<LuaVector3>(L, 2);
    float dot = self->Dot(*other);
    lua_pushnumber(L, dot);
    return 1;
}

int LuaVector3::luaCross(lua_State* L) {
    LuaVector3* vec = check<LuaVector3>(L, 1);
    LuaVector3* other = check<LuaVector3>(L, 2);

    lua_pop(L, 2); // Pop the two userdata from the stack to create new ones
    LuaVector3 cross = vec->Cross(*other);
    lua_pushnumber(L, cross.GetX());
    lua_pushnumber(L, cross.GetY());
    lua_pushnumber(L, cross.GetZ());
    return luaNew(L);
}

int LuaVector3::luaGetLength(lua_State* L) {
    LuaVector3* vec = check<LuaVector3>(L, 1);
    lua_pushnumber(L, vec->GetLength());
    return 1;
}

// Creates a normalized copy of the vector and returns it
int LuaVector3::luaNormalize(lua_State* L) {
    LuaVector3* vec = check<LuaVector3>(L, 1);
    lua_pop(L, 1); // Pop the userdata from the stack to create new ones
    LuaVector3 normalized = vec->Normalize();
    lua_pushnumber(L, normalized.GetX());
    lua_pushnumber(L, normalized.GetY());
    lua_pushnumber(L, normalized.GetZ());
    return luaNew(L);
}

int LuaVector3::luaNew(lua_State* L) {
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    float z = luaL_checknumber(L, 3);
    LuaUtil::Get().CreateAndPush<LuaVector3>(L, x, y, z);
    return 1;
}

int LuaVector3::lua__add(lua_State* L) {
    LuaVector3* vec1 = check<LuaVector3>(L, 1);
    LuaVector3* vec2 = check<LuaVector3>(L, 2);

    LuaUtil::Get().CreateAndPush<LuaVector3>(L, vec1->Get() + vec2->Get());
    return 1;
}

int LuaVector3::lua__sub(lua_State* L) {
    LuaVector3* vec1 = check<LuaVector3>(L, 1);
    LuaVector3* vec2 = check<LuaVector3>(L, 2);

    LuaUtil::Get().CreateAndPush<LuaVector3>(L, vec1->Get() - vec2->Get());
    return 1;
}

int LuaVector3::lua__mul(lua_State* L) {
    LuaVector3* vec1 = check<LuaVector3>(L, 1);

    if (lua_isnumber(L, 2)) {
        // Scalar multiplication
        float scalar = luaL_checknumber(L, 2);
        LuaUtil::Get().CreateAndPush<LuaVector3>(L, vec1->Get() * scalar);
    } else {
        // Component-wise vector multiplication
        LuaVector3* vec2 = check<LuaVector3>(L, 2);
        LuaUtil::Get().CreateAndPush<LuaVector3>(
            L, vec1->Get() * vec2->Get()); // Multiplies every component
    }
    return 1;
}

int LuaVector3::lua__div(lua_State* L) {
    LuaVector3* vec1 = check<LuaVector3>(L, 1);

    if (lua_isnumber(L, 2)) {
        // Scalar division
        float scalar = luaL_checknumber(L, 2);
        LuaUtil::Get().CreateAndPush<LuaVector3>(L, vec1->Get() / scalar);
    } else {
        // Component-wise vector division
        LuaVector3* vec2 = check<LuaVector3>(L, 2);
        LuaUtil::Get().CreateAndPush<LuaVector3>(
            L, vec1->Get() / vec2->Get()); // Divides every component
    }
    return 1;
}

int LuaVector3::lua__eq(lua_State* L) {
    LuaVector3* vec1 = check<LuaVector3>(L, 1);
    LuaVector3* vec2 = check<LuaVector3>(L, 2);

    bool equal = (vec1->GetX() == vec2->GetX()) &&
                 (vec1->GetY() == vec2->GetY()) &&
                 (vec1->GetZ() == vec2->GetZ());
    lua_pushboolean(L, equal);
    return 1;
}
int LuaVector3::lua__tostring(lua_State* L) {
    LuaVector3* vec = check<LuaVector3>(L, 1);
    std::string str = "Vector3(" + std::to_string(vec->GetX()) + ", " +
                      std::to_string(vec->GetY()) + ", " +
                      std::to_string(vec->GetZ()) + ")";
    lua_pushstring(L, str.c_str());
    return 1;
}
