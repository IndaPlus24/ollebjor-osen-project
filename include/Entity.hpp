#pragma once

#include <Jolt/Jolt.h>
#include "Jolt/Physics/Body/BodyInterface.h"
#include <Jolt/Physics/Body/BodyID.h>
#include <glm/glm.hpp>
#include <bgfx/bgfx.h>
#include "Enums.hpp"
#include "PhysicsCore.hpp"
#include "Texture.hpp"
#include <glm/ext/matrix_transform.hpp>

class Entity {
  protected:
    RigidBodyType bodyType = RigidBodyType::Static;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 size;
    glm::mat4 transform;
    bgfx::TextureHandle textureHandle;

    bgfx::DynamicVertexBufferHandle vbh;
    bgfx::IndexBufferHandle ibh;

    JPH::BodyID bodyID;
    JPH::BodyInterface* bodyInterface = nullptr;

    void QuaternionRotate(glm::mat4& result, const glm::vec3& axis,
                          float angle);

  public:
    Entity(RigidBodyType bodyType, PhysicsCore& physicsCore,
           bgfx::VertexLayout& layout, Texture& texture,
           glm::vec3 position = glm::vec3(0.0f),
           glm::vec3 rotation = glm::vec3(0.0f),
           glm::vec3 size = glm::vec3(1.0f));
    Entity(Entity&& other) noexcept;
    Entity& operator=(Entity&& other) noexcept;
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    ~Entity();

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

    bgfx::TextureHandle SetTexture();
    void UpdateTexture(Texture& texture);

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
    inline glm::vec3 GetPosition() const { return position; }
    inline glm::vec3 GetRotation() const { return rotation; }
    inline glm::vec3 GetSize() const { return size; }
    inline glm::mat4 GetTransform() const { return transform; }

    virtual void UpdateMesh(PhysicsCore& physicsCore, bgfx::VertexLayout& layout) = 0;
};
