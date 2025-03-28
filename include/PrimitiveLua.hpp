#pragma once
#include <string>
#include <glm/glm.hpp>


class PrimitiveLua {
    public:
        PrimitiveLua(const std::string& name);
        PrimitiveLua();
        ~PrimitiveLua();

        const std::string& GetName() const;
        
        void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
        const uint32_t& GetColor() const;

        void SetPosition(glm::vec3& position);
        const glm::vec3& GetPosition() const;

    private:
        std::string name; //TODO: Make enum
        glm::vec3 position;
        uint32_t color;
};