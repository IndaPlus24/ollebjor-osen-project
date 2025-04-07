#pragma once
#include <string>
#include <glm/glm.hpp>
#include <lua.hpp>

class PrimitiveLua {
  public:
    PrimitiveLua(const std::string& name);
    PrimitiveLua();
    ~PrimitiveLua();

    std::string& GetName();
    void SetName() const;

    // void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    // const uint32_t& GetColor() const;

    void SetPosition(glm::vec3& position);
    glm::vec3& GetPosition();

    const static luaL_Reg methods[];
    const static luaL_Reg functions[];
    const static std::string luaName;
    const static std::string metatableName;
    const static lua_CFunction luaIndexPrimitive;
    const static lua_CFunction luaNewIndexPrimitive;

  private:
    std::string name; // TODO: Make enum
    glm::vec3 position;
    uint32_t color;
};
