#pragma once

#include "Enums.hpp"
#include "bgfx/bgfx.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>

class Primitive {
  private:
    PrimitiveType type;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 size;
    glm::mat4 transform;

    void GetPrimitiveTypeData(const bgfx::Memory* &vertMem,
                              const bgfx::Memory* &indiMem, PrimitiveType type);

  public:
    Primitive(PrimitiveType type, bgfx::VertexLayout& layout,
              glm::vec3 position = glm::vec3(0.0f),
              glm::vec3 rotation = glm::vec3(0.0f),
              glm::vec3 size = glm::vec3(1.0f));
    Primitive(bgfx::VertexLayout& layout);
    Primitive(Primitive&& other) noexcept;
    Primitive(const Primitive&) = default;
    Primitive& operator=(Primitive&&) = default;
    Primitive& operator=(const Primitive&) = default;
    ~Primitive();

    bgfx::VertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;
    const bgfx::Memory* verticesMem;
    const bgfx::Memory* indicesMem;

    inline void SetVertexBuffer() { bgfx::setVertexBuffer(0, vbh); }
    inline void SetIndexBuffer() { bgfx::setIndexBuffer(ibh); }

    inline void ApplyTransform() { bgfx::setTransform(&transform[0][0]); }

    inline void SetPosition(glm::vec3 position) {
        this->position = position;
        transform = glm::translate(glm::mat4(1.0f), position);
    }

    void SetRotation(glm::vec3 rotation);

    inline void SetSize(glm::vec3 size) {
        this->size = size;
        transform = glm::scale(transform, size);
    }
};

