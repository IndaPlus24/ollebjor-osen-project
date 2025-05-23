#pragma once
#include <glm/glm.hpp>
#include <lua.hpp>

class LuaVector3 {
  public:
    LuaVector3();
    LuaVector3(glm::vec3 vec);
    LuaVector3(float x, float y, float z);
    ~LuaVector3();

    float GetX() const;
    float GetY() const;
    float GetZ() const;

    float GetLength() const;
    float Dot(const LuaVector3& other) const;
    LuaVector3 Cross(const LuaVector3& other) const;
    LuaVector3 Normalize() const;

    glm::vec3 Get() const;
    void Set(const glm::vec3& vec);

    // TODO: can we make these methods in the luaexporter?
    static int luaGetX(lua_State* L);
    static int luaGetY(lua_State* L);
    static int luaGetZ(lua_State* L);
    static int luaGetLength(lua_State* L);
    static int luaDot(lua_State* L);
    static int luaCross(lua_State* L);
    static int luaNormalize(lua_State* L);
    static int luaNew(lua_State* L);
    static int lua__add(lua_State* L);
    static int lua__sub(lua_State* L);
    static int lua__mul(lua_State* L);
    static int lua__div(lua_State* L);
    static int lua__eq(lua_State* L);
    static int lua__tostring(lua_State* L);

  private:
    glm::vec3 position;
};