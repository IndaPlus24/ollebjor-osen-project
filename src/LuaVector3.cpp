#include "LuaVector3.hpp"
#include "lauxlib.h"
#include "lua.h"
#include <iostream>
#include <stdexcept>

LuaVector3::LuaVector3() : position(0.0f, 0.0f, 0.0f) {}
LuaVector3::LuaVector3(glm::vec3 position) : position(position) {}

LuaVector3::~LuaVector3() {
    // Destructor logic if needed
    std::cout << "LuaVector3 destructor called for vector" << std::endl;
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

// int LuaVector3::luaGetX(lua_State* L) {
//     LuaVector3* vec = (LuaVector3*)luaL_checkudata(L, 1, "LuaVector3.Metatable");
//     luaL_argcheck(L, vec != nullptr, 1, "'LuaVector3' expected in luaGetX");
//     lua_pushnumber(L, vec->GetX());
//     return 1;
// }
int LuaVector3::luaDot(lua_State *L){
    LuaVector3* vec = (LuaVector3*)luaL_checkudata(L, 1, "Vector3.Metatable");
    LuaVector3* other = (LuaVector3*)luaL_checkudata(L, 2, "Vector3.Metatable");
    
    float dot = vec->Dot(*other);
    lua_pushnumber(L, dot);
    return 1;
}


int LuaVector3::luaNewVector3(lua_State *L) {
    float x = luaL_checknumber(L, 1);
    float y = luaL_checknumber(L, 2);
    float z = luaL_checknumber(L, 3);
    std::cout << "Creating new LuaVector3: " << x << ", " << y << ", " << z << std::endl;
    LuaVector3* vec = (LuaVector3*)lua_newuserdata(L, sizeof(LuaVector3));
    new (vec) LuaVector3(glm::vec3(x, y, z)); // Placement new
    luaL_setmetatable(L, "Vector3.Metatable");
    return 1;
}