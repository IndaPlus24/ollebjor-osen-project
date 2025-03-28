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
    uint32_t abgr;

    bgfx::DynamicVertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;

    void GetPrimitiveTypeData(const bgfx::Memory* &vertMem,
                              const bgfx::Memory* &indiMem, PrimitiveType type, uint32_t abgr = 0xffffff);

  public:
    Primitive(PrimitiveType type, bgfx::VertexLayout& layout,
              uint32_t abgr = 0xffffff,
              glm::vec3 position = glm::vec3(0.0f),
              glm::vec3 rotation = glm::vec3(0.0f),
              glm::vec3 size = glm::vec3(1.0f));
    Primitive(bgfx::VertexLayout& layout);
    Primitive(Primitive&& other) noexcept;
    Primitive(const Primitive&) = default;
    Primitive& operator=(Primitive&&) = default;
    Primitive& operator=(const Primitive&) = default;
    ~Primitive();

    inline void SetVertexBuffer() { bgfx::setVertexBuffer(0, vbh); }
    inline void SetIndexBuffer() { bgfx::setIndexBuffer(ibh); }

    inline void ApplyTransform() { bgfx::setTransform(&transform[0][0]); }

    void SetColor(uint32_t abgr);

    inline void SetPosition(glm::vec3 position) {
        this->position = position;
        transform = glm::translate(glm::mat4(1.0f), position);
    }
    inline void AddPosition(glm::vec3 position) {
        this->position += position;
        transform = glm::translate(transform, position);
    }

    void SetRotation(glm::vec3 rotation);
    void AddRotation(glm::vec3 rotation);

    inline void SetSize(glm::vec3 size) {
        this->size = size;
        transform = glm::scale(transform, size);
    }
};

