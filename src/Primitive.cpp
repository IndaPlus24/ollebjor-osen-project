#include "Primitive.hpp"
#include "Jolt/Math/Vec3.h"
#include "Jolt/Physics/Body/BodyID.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "PhysicsCore.hpp"
#include "PrimitiveDefinitions.hpp"
#include "Enums.hpp"
#include "Texture.hpp"
#include "utils.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bx/debug.h"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/trigonometric.hpp"
#include <cstddef>

Primitive::Primitive(PrimitiveType type, RigidBodyType bodyType,
                     PhysicsCore& physicsCore, bgfx::VertexLayout& layout,
                     Texture& texture, glm::vec3 position, glm::vec3 rotation,
                     glm::vec3 size)
    : type(type), position(position), rotation(rotation), size(size),
      bodyType(bodyType) {
    const bgfx::Memory* verticesMem = nullptr;
    const bgfx::Memory* indicesMem = nullptr;
    GetPrimitiveTypeData(verticesMem, indicesMem, type);

    vbh = bgfx::createDynamicVertexBuffer(verticesMem, layout);
    ibh = bgfx::createIndexBuffer(indicesMem);
    if (vbh.idx == bgfx::kInvalidHandle || ibh.idx == bgfx::kInvalidHandle) {
        bx::debugPrintf("Failed to create primitive: Type: %d vbh: %d ibh: %x",
                        type, vbh.idx, ibh.idx);
    } else {
        bx::debugPrintf("Primitive created: Type: %d vbh: %d ibh: %d", type,
                        vbh.idx, ibh.idx);
    }
    textureHandle = texture.GetTextureHandle();

    SetPosition(position);
    SetRotation(rotation);
    SetSize(size);

    // setup physics
    bodyInterface = &physicsCore.GetBodyInterface();

    JPH::Vec3 joltPosition(position.x, position.y, position.z);
    JPH::Vec3 joltSize(size.x, size.y, size.z);
    switch (bodyType) {
    case RigidBodyType::Static: {
        glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 rotatedNormal =
            glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x),
                        glm::vec3(1.0f, 0.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y),
                        glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z),
                        glm::vec3(0.0f, 0.0f, 1.0f)) *
            glm::vec4(normal, 1.0f);
        JPH::Vec3 joltNormal(rotatedNormal.x, rotatedNormal.y, rotatedNormal.z);
        bodyID = physicsCore.AddStaticPlane(joltPosition, joltNormal);
        bx::debugPrintf("Static Plane created: Type: %d vbh: %d ibh: %d", type,
                        vbh.idx, ibh.idx);

    } break;
    case RigidBodyType::Dynamic: {
        switch (type) {
        case PrimitiveType::Cube: {
            bodyID = physicsCore.AddDynamicBox(joltPosition, joltSize, 1.0f);
            bx::debugPrintf("Dynamic Cube created: Type: %d vbh: %d ibh: %d",
                            type, vbh.idx, ibh.idx);
        } break;
        case PrimitiveType::Sphere: {
            bodyID =
                physicsCore.AddDynamicSphere(size.x * 1.96, joltPosition, 1.0f);
            bx::debugPrintf("Dynamic Sphere created: Type: %d vbh: %d ibh: %d",
                            type, vbh.idx, ibh.idx);
        } break;
        }
    }
    }
}

Primitive::Primitive(Primitive&& other) noexcept {
    type = other.type;
    bodyType = other.bodyType;
    position = other.position;
    rotation = other.rotation;
    textureHandle = other.textureHandle;
    size = other.size;
    vbh = other.vbh;
    ibh = other.ibh;
    transform = other.transform;
    bodyInterface = other.bodyInterface;
    bodyID = other.bodyID;
    other.vbh.idx = bgfx::kInvalidHandle;
    other.ibh.idx = bgfx::kInvalidHandle;
    other.bodyInterface = nullptr;
    other.bodyID = JPH::BodyID();
    other.bodyType = RigidBodyType::Static;
    bx::debugPrintf("Primitive moved: Type: %d vbh: %d ibh: %d", type, vbh.idx,
                    ibh.idx);
}

Primitive::~Primitive() {
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
    if (isInvalidIBH && isInvalidVBH && isInvalidBodyID) {
        bx::debugPrintf("Empty Primitive destroy: Type: %d vbh: %d ibh: %d",
                        type, vbh.idx, ibh.idx);
        return;
    }
    bx::debugPrintf("Primitive destroyed: Type: %d vbh: %d ibh: %d", type,
                    vbh.idx, ibh.idx);
}

bgfx::TextureHandle Primitive::SetTexture() { return textureHandle; }

void Primitive::UpdateTexture(Texture& texture) {
    this->textureHandle = texture.GetTextureHandle();
}

void Primitive::SetPhysicsPosition(glm::vec3 position,
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

void Primitive::AddImpulse(glm::vec3 impulse) {
    if (bodyInterface) {
        bodyInterface->AddImpulse(bodyID, ToJPH(impulse));
    }
}

// From: https://stackoverflow.com/a/66054048
// Now with the quaternion transform you rotate any vector or compound it with
// another transformation matrix
void Primitive::QuaternionRotate(glm::mat4& result, const glm::vec3& axis,
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

void Primitive::SetRotation(glm::vec3 rotation) {
    if (rotation == this->rotation) {
        return;
    }
    this->rotation = rotation;
    QuaternionRotate(transform, glm::vec3(1.0f, 0.0f, 0.0f), rotation.x);
    QuaternionRotate(transform, glm::vec3(0.0f, 1.0f, 0.0f), rotation.y);
    QuaternionRotate(transform, glm::vec3(0.0f, 0.0f, 1.0f), rotation.z);
}

void Primitive::AddRotation(glm::vec3 rotation) {
    this->rotation += rotation;
    QuaternionRotate(transform, glm::vec3(1.0f, 0.0f, 0.0f), rotation.x);
    QuaternionRotate(transform, glm::vec3(0.0f, 1.0f, 0.0f), rotation.y);
    QuaternionRotate(transform, glm::vec3(0.0f, 0.0f, 1.0f), rotation.z);
}

void Primitive::GetPrimitiveTypeData(const bgfx::Memory*& vertMem,
                                     const bgfx::Memory*& indiMem,
                                     PrimitiveType type) {
    switch (type) {
    case PrimitiveType::Cube: {
        PrimitiveCube cube;
        vertMem = bgfx::copy(cube.vertices, sizeof(cube.vertices));
        indiMem = bgfx::copy(cube.indices, sizeof(cube.indices));
    } break;
    case PrimitiveType::Plane: {
        PrimitiveQuad quad;
        vertMem = bgfx::copy(quad.vertices, sizeof(quad.vertices));
        indiMem = bgfx::copy(quad.indices, sizeof(quad.indices));
    } break;
    case PrimitiveType::Sphere: {
        // PrimitiveSphere sphere;
        // vertMem = bgfx::copy(sphere.vertices, sizeof(sphere.vertices));
        // indiMem = bgfx::copy(sphere.indices, sizeof(sphere.indices));
    } break;
    }
    BX_ASSERT(vertMem->data != nullptr, "Vertex Memory is null");
    BX_ASSERT(vertMem->size != 0, "Vertex Memory size is 0");
    BX_ASSERT(indiMem->data != nullptr, "Index Memory is null");
    BX_ASSERT(indiMem->size != 0, "Vertex Memory size is 0");
}
