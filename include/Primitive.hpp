#pragma once

#include "Enums.hpp"
#include "utils.hpp"
#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/BodyID.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "PhysicsCore.hpp"
#include "bgfx/bgfx.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>

class Primitive {
  private:
    PrimitiveType type;
    RigidBodyType bodyType = RigidBodyType::Static;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 size;
    glm::mat4 transform;
    uint32_t abgr;

    bgfx::DynamicVertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;

    JPH::BodyID bodyID;
    JPH::BodyInterface* bodyInterface = nullptr;

    void GetPrimitiveTypeData(const bgfx::Memory*& vertMem,
                              const bgfx::Memory*& indiMem, PrimitiveType type,
                              uint32_t abgr = 0xffffff);
    void QuaternionRotate(glm::mat4& result, const glm::vec3& axis,
                          float angle);

  public:
    Primitive(PrimitiveType type, RigidBodyType bodyType,
              PhysicsCore& physicsCore, bgfx::VertexLayout& layout,
              uint32_t abgr = 0xffffff, glm::vec3 position = glm::vec3(0.0f),
              glm::vec3 rotation = glm::vec3(0.0f),
              glm::vec3 size = glm::vec3(1.0f));
    Primitive(Primitive&& other) noexcept;
    Primitive(const Primitive&) = default;
    Primitive& operator=(Primitive&&) = default;
    Primitive& operator=(const Primitive&) = default;
    ~Primitive();

    inline void SetVertexBuffer() { bgfx::setVertexBuffer(0, vbh); }
    inline void SetIndexBuffer() { bgfx::setIndexBuffer(ibh); }
    inline void SetTransform(const glm::mat4& transform) {
        this->transform = transform;
        this->position = glm::vec3(transform[3]);
        this->rotation = glm::vec3(transform[2]);
        this->size =
            glm::vec3(transform[0][0], transform[1][1], transform[2][2]);
    }

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
    inline void SetScale(glm::vec3 scale) {
        this->size = scale;
        transform = glm::scale(transform, scale);
    }

    void SetPhysicsPosition(glm::vec3 position, JPH::EActivation activation =
                                                    JPH::EActivation::Activate);

    void AddImpulse(glm::vec3 impulse);

    void SetRotation(glm::vec3 rotation);
    void AddRotation(glm::vec3 rotation);

    inline void SetSize(glm::vec3 size) {
        this->size = size;
        transform = glm::scale(transform, size);
    }

    inline JPH::BodyID GetBodyID() const { return bodyID; }
    inline RigidBodyType GetBodyType() const { return bodyType; }
};
