#pragma once
#include <string>
#include <glm/glm.hpp>
#include "LuaClass.hpp"
#include "lauxlib.h"
#define PRIMITIVE_METHOD_COUNT 3

class PrimitiveLua : public LuaClass {
    public:
        PrimitiveLua(const std::string& name);
        PrimitiveLua();
        ~PrimitiveLua();

        // const std::string& GetName() const;
        
        // void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        // const uint32_t& GetColor() const;

        void SetPosition(glm::vec3& position);
        const glm::vec3& GetPosition() const;

        const static luaL_Reg methods[];

    private:
        std::string name; //TODO: Make enum
        glm::vec3 position;
        uint32_t color;
};