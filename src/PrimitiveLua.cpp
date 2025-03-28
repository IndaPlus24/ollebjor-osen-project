#include "PrimitiveLua.hpp"

PrimitiveLua::PrimitiveLua(const std::string& name) : name(name) {
    // Initialize position to (0, 0, 0) and color to white (0xffffffff)
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    color = 0xffffffff;
}

PrimitiveLua::PrimitiveLua() : name(""), position(0.0f, 0.0f, 0.0f), color(0xffffffff) {
}

PrimitiveLua::~PrimitiveLua() {
    // Destructor logic if needed
}

const std::string& PrimitiveLua::GetName() const {
    return name;
}

void PrimitiveLua::SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    color = (r << 24) | (g << 16) | (b << 8) | a;
}

const uint32_t& PrimitiveLua::GetColor() const {
    return color;
}

void PrimitiveLua::SetPosition(glm::vec3& position) {
    this->position = position;
}

const glm::vec3& PrimitiveLua::GetPosition() const {
    return position;
}