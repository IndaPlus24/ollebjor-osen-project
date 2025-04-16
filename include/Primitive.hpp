#pragma once

#include "Entity.hpp"
#include "Enums.hpp"
#include "Texture.hpp"
#include "PhysicsCore.hpp"
#include "bgfx/bgfx.h"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>

class Primitive: public Entity {
  private:
    PrimitiveType type;

    void GetPrimitiveTypeData(const bgfx::Memory*& vertMem,
                              const bgfx::Memory*& indiMem, PrimitiveType type);

  public:
    Primitive(PrimitiveType type, RigidBodyType bodyType,
              PhysicsCore& physicsCore, bgfx::VertexLayout& layout,
              Texture& texture, glm::vec3 position = glm::vec3(0.0f),
              glm::vec3 rotation = glm::vec3(0.0f),
              glm::vec3 size = glm::vec3(1.0f));
    Primitive(Primitive&& other) noexcept;
    Primitive& operator=(Primitive&& other) noexcept;
    Primitive(const Primitive&) = delete;
    Primitive& operator=(const Primitive&) = delete;
    ~Primitive();

    inline PrimitiveType GetType() const { return type; }
    inline void SetType(PrimitiveType type) { this->type = type; } //FIXME
};
