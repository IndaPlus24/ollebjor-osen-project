#include "Entity.hpp"
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>

#include <bgfx/bgfx.h>
#include "utils.hpp"

Entity::Entity(RigidBodyType bodyType, PhysicsCore& physicsCore,
               bgfx::VertexLayout& layout, Texture& texture, glm::vec3 position,
               glm::vec3 rotation, glm::vec3 size)
    : bodyType(bodyType), position(position), rotation(rotation), size(size),
      textureHandle(texture.GetTextureHandle()) {}

Entity::Entity(Entity&& other) noexcept {
    bodyType = other.bodyType;
    position = other.position;
    rotation = other.rotation;
    size = other.size;
    transform = other.transform;
    textureHandle = other.textureHandle;
    vbh = other.vbh;
    ibh = other.ibh;
    bodyID = other.bodyID;
    bodyInterface = other.bodyInterface;

    other.bodyInterface = nullptr;
    other.bodyID = JPH::BodyID();
    other.vbh.idx = bgfx::kInvalidHandle;
    other.ibh.idx = bgfx::kInvalidHandle;
    other.textureHandle.idx = bgfx::kInvalidHandle;
}

Entity& Entity::operator=(Entity&& other) noexcept {
    if (this != &other) {
        bodyType = other.bodyType;
        position = other.position;
        rotation = other.rotation;
        size = other.size;
        transform = other.transform;
        textureHandle = other.textureHandle;
        vbh = other.vbh;
        ibh = other.ibh;
        bodyID = other.bodyID;
        bodyInterface = other.bodyInterface;

        other.bodyInterface = nullptr;
        other.bodyID = JPH::BodyID();
        other.vbh.idx = bgfx::kInvalidHandle;
        other.ibh.idx = bgfx::kInvalidHandle;
    }
    return *this;
}

Entity::~Entity() {
    bool isInvalidVBH = vbh.idx == bgfx::kInvalidHandle;
    bool isInvalidIBH = ibh.idx == bgfx::kInvalidHandle;
    bool isInvalidBodyID = bodyID == JPH::BodyID();
    if (!isInvalidVBH) {
        bgfx::destroy(vbh);
    }
    if (!isInvalidIBH) {
        bgfx::destroy(ibh);
    }
    if (!isInvalidBodyID) {
        bodyInterface->RemoveBody(bodyID);
        bodyInterface->DestroyBody(bodyID);
    }
}

// From: https://stackoverflow.com/a/66054048
// Now with the quaternion transform you rotate any vector or compound it with
// another transformation matrix
void Entity::QuaternionRotate(glm::mat4& result, const glm::vec3& axis,
                              float angle) {
    float angleRad = glm::radians(angle);
    const auto& axisNorm = glm::normalize(axis);

    float w = glm::cos(angleRad / 2);
    float v = glm::sin(angleRad / 2);
    glm::vec3 qv = axisNorm * v;

    glm::quat quaternion = glm::quat(w, qv);
    glm::mat4 quatTransform = glm::mat4_cast(quaternion);
    result *= quatTransform;
}

bgfx::TextureHandle Entity::SetTexture() { return textureHandle; }

void Entity::UpdateTexture(Texture& texture) {
    this->textureHandle = texture.GetTextureHandle();
}

void Entity::SetPhysicsPosition(glm::vec3 position,
                                JPH::EActivation activation) {
    if (this->position == position) {
        return;
    }
    this->position = position;
    transform = glm::translate(glm::mat4(1.0f), position);
    if (bodyInterface) {
        bodyInterface->SetPosition(bodyID, ToJPH(position), activation);
    }
}

void Entity::AddImpulse(glm::vec3 impulse) {
    if (bodyInterface) {
        bodyInterface->AddImpulse(bodyID, ToJPH(impulse));
    }
}

void Entity::SetRotation(glm::vec3 rotation) {
    if (rotation == this->rotation) {
        return;
    }
    this->rotation = rotation;
    QuaternionRotate(transform, glm::vec3(1.0f, 0.0f, 0.0f), rotation.x);
    QuaternionRotate(transform, glm::vec3(0.0f, 1.0f, 0.0f), rotation.y);
    QuaternionRotate(transform, glm::vec3(0.0f, 0.0f, 1.0f), rotation.z);
}

void Entity::AddRotation(glm::vec3 rotation) {
    this->rotation += rotation;
    QuaternionRotate(transform, glm::vec3(1.0f, 0.0f, 0.0f), rotation.x);
    QuaternionRotate(transform, glm::vec3(0.0f, 1.0f, 0.0f), rotation.y);
    QuaternionRotate(transform, glm::vec3(0.0f, 0.0f, 1.0f), rotation.z);
}
