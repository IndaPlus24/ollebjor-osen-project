#include "LuaVector3.hpp"
#include "lauxlib.h"
#include "lua.h"

LuaVector3::LuaVector3() : position(0.0f, 0.0f, 0.0f) {}
LuaVector3::LuaVector3(glm::vec3 position) : position(position) {}

LuaVector3::~LuaVector3() {
    // Destructor logic if needed
    // std::cout << "LuaVector3 destructor called for vector" << std::endl;
}

void LuaVector3::SetPosition(glm::vec3& position) { this->position = position; }
glm::vec3& LuaVector3::GetPosition() { return position; }

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

int LuaVector3::luaGetX(lua_State* L) {
    int i = lua_upvalueindex(1);
    const char* mtName = lua_tostring(L, i);
    LuaVector3* vec = (LuaVector3*)luaL_checkudata(L, 1, mtName);
    lua_pushnumber(L, vec->GetX());
    return 1;
}

int LuaVector3::luaGetY(lua_State* L) {
    int i = lua_upvalueindex(1);
    const char* mtName = lua_tostring(L, i);
    LuaVector3* vec = (LuaVector3*)luaL_checkudata(L, 1, mtName);
    lua_pushnumber(L, vec->GetY());
    return 1;
}

int LuaVector3::luaGetZ(lua_State* L) {
    int i = lua_upvalueindex(1);
    const char* mtName = lua_tostring(L, i);
    LuaVector3* vec = (LuaVector3*)luaL_checkudata(L, 1, mtName);
    lua_pushnumber(L, vec->GetZ());
    return 1;
}

int LuaVector3::luaDot(lua_State* L) {
    int i = lua_upvalueindex(1);
    const char* mtName = lua_tostring(L, i);
    LuaVector3* vec = (LuaVector3*)luaL_checkudata(L, 1, mtName);
    LuaVector3* other = (LuaVector3*)luaL_checkudata(L, 2, mtName);

    float dot = vec->Dot(*other);
    lua_pushnumber(L, dot);
    return 1;
}

int LuaVector3::luaCross(lua_State* L) {
    int i = lua_upvalueindex(1);
    const char* mtName = lua_tostring(L, i);
    LuaVector3* vec = (LuaVector3*)luaL_checkudata(L, 1, mtName);
    LuaVector3* other = (LuaVector3*)luaL_checkudata(L, 2, mtName);

    lua_pop(L, 2); // Pop the two userdata from the stack to create new ones
    LuaVector3 cross = vec->Cross(*other);
    lua_pushnumber(L, cross.GetX());
    lua_pushnumber(L, cross.GetY());
    lua_pushnumber(L, cross.GetZ());
    return luaNewVector3(L);
}

int LuaVector3::luaNewVector3(lua_State* L) {
    int i = lua_upvalueindex(1);
    const char* mtName = lua_tostring(L, i);
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    float z = luaL_checknumber(L, 3);
    LuaVector3* vec = (LuaVector3*)lua_newuserdata(L, sizeof(LuaVector3));
    new (vec) LuaVector3(glm::vec3(x, y, z)); // Placement new
    luaL_setmetatable(L, mtName);
    return 1;
}

int LuaVector3::luaGetLength(lua_State* L) {
    int i = lua_upvalueindex(1);
    const char* mtName = lua_tostring(L, i);
    LuaVector3* vec = (LuaVector3*)luaL_checkudata(L, 1, mtName);
    lua_pushnumber(L, vec->GetLength());
    return 1;
}