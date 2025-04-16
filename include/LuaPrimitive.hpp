#pragma once
#include "Entity.hpp"
#include "Enums.hpp"
#include "LuaVector3.hpp"
#include <glm/glm.hpp>
#include <lua.hpp>
#include "Primitive.hpp"
#include "SceneManager.hpp"

class LuaPrimitive {
  public:
    LuaPrimitive(PrimitiveType type);
    LuaPrimitive(PrimitiveType type, LuaVector3& position);
    ~LuaPrimitive();

    PrimitiveType GetType();
    void SetType(PrimitiveType type);
    // void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    // const uint32_t& GetColor() const;
    void SetPosition(LuaVector3& position);
    LuaVector3 GetPosition();

    static int luaGetType(lua_State* L);
    static int luaSetType(lua_State* L);
    static int luaGetPosition(lua_State* L);
    static int luaSetPosition(lua_State* L);
    static int luaNew(lua_State* L);

  private:
    SceneRef<Entity> m_ref;
};
