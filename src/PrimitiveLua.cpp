#include "PrimitiveLua.hpp"
#include "LuaClass.hpp"
#include "bx/debug.h"
#include "glm/fwd.hpp"
#include "lua.h"
#include <lua.hpp>
#include <iostream>
#include <glm/glm.hpp>

PrimitiveLua::PrimitiveLua(const std::string& name) : name(name) {
    // Initialize position to (0, 0, 0) and color to white (0xffffffff)
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    color = 0xffffffff;
}

namespace {
int luaSetPosition(lua_State* L) {
    PrimitiveLua* i = (PrimitiveLua*)lua_touserdata(L, 1);

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int z = luaL_checkinteger(L, 4);
    glm::vec3 position = glm::vec3(x, y, z);
    i->SetPosition(position);

    return 0;
}
} // namespace

namespace {
int luaGetPosition(lua_State* L) {
    PrimitiveLua* i = (PrimitiveLua*)lua_touserdata(L, 1);
    
    const glm::vec3& pos = i->GetPosition();
    lua_pushnumber(L, pos.x);
    lua_pushnumber(L, pos.y);
    lua_pushnumber(L, pos.z);
    return 3;
}
} // namespace

namespace {
int newPrimitive(lua_State* L) {
    // Create a new instance of PrimitiveLua and push it onto the stack
    // PrimitiveLua* prim = reinterpret_cast<PrimitiveLua*>(lua_newuserdatauv(
    //     L, sizeof(PrimitiveLua), 0));     // Allocate memory for the instance
    // *prim = PrimitiveLua("PrimitiveLua"); // Initialize the instance

    PrimitiveLua* obj = new PrimitiveLua();
    PrimitiveLua** userdata =
        reinterpret_cast<PrimitiveLua**>(lua_newuserdatauv(L, sizeof(obj), 0));
    *userdata = obj;
    return 1;
}
} // namespace

PrimitiveLua::PrimitiveLua()
    : name(""), position(0.0f, 0.0f, 0.0f), color(0xffffffff) {}

PrimitiveLua::~PrimitiveLua() {
    // Destructor logic if needed
}

// const std::string& PrimitiveLua::GetName() const {
//     return name;
// }

// void PrimitiveLua::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
//     color = (r << 24) | (g << 16) | (b << 8) | a;
// }

// const uint32_t& PrimitiveLua::GetColor() const {
//     return color;
// }

void PrimitiveLua::SetPosition(glm::vec3& position) {
    this->position = position;
}

const glm::vec3& PrimitiveLua::GetPosition() const { return position; }

const luaL_Reg PrimitiveLua::methods[] = {
    {"SetPosition", &luaSetPosition},
    {"GetPosition", &luaGetPosition},
    {"new", &newPrimitive},
    {nullptr, nullptr}}; // End of the methods array