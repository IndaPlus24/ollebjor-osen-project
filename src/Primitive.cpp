#include "Primitive.hpp"
#include "Entity.hpp"
#include "Jolt/Math/Vec3.h"
#include "Jolt/Physics/Body/BodyID.h"
#include "PhysicsCore.hpp"
#include "PrimitiveDefinitions.hpp"
#include "Enums.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bx/debug.h"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "utils.hpp"

Primitive::Primitive(PrimitiveType type, RigidBodyType bodyType,
                     PhysicsCore& physicsCore, bgfx::VertexLayout& layout,
                     uint64_t materialId, glm::vec3 position, glm::vec3 rotation,
                     glm::vec3 size)
    : Entity(bodyType, physicsCore, layout, materialId, position, rotation, size) {
    const bgfx::Memory* verticesMem = nullptr;
    const bgfx::Memory* indicesMem = nullptr;
    GetPrimitiveTypeData(verticesMem, indicesMem, type);

    vbh = bgfx::createDynamicVertexBuffer(verticesMem, layout);
    ibh = bgfx::createIndexBuffer(indicesMem);
    if (vbh.idx == bgfx::kInvalidHandle || ibh.idx == bgfx::kInvalidHandle) {
        bx::debugPrintf("Failed to create primitive: Type: %d vbh: %d ibh: %x\n",
                        type, vbh.idx, ibh.idx);
    } else {
        bx::debugPrintf("Primitive created: Type: %d vbh: %d ibh: %d\n", type,
                        vbh.idx, ibh.idx);
    }

    SetPosition(position);
    SetRotation(rotation);
    SetSize(size);

    // setup physics
    bodyInterface = &physicsCore.GetBodyInterface();

    JPH::Vec3 joltPosition = ToJPH(position);
    JPH::Vec3 joltSize = ToJPH(size);
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
        bx::debugPrintf("Static Plane created: Type: %d vbh: %d ibh: %d\n", type,
                        vbh.idx, ibh.idx);

    } break;
    case RigidBodyType::Dynamic: {
        switch (type) {
        case PrimitiveType::Cube: {
            bodyID = physicsCore.AddDynamicBox(joltPosition, joltSize, 1.0f);
            bx::debugPrintf("Dynamic Cube created: Type: %d vbh: %d ibh: %d\n",
                            type, vbh.idx, ibh.idx);
        } break;
        case PrimitiveType::Sphere: {
            bodyID = physicsCore.AddDynamicSphere(size.x, joltPosition, 1.0f);
            bx::debugPrintf("Dynamic Sphere created: Type: %d vbh: %d ibh: %d\n",
                            type, vbh.idx, ibh.idx);
        } break;
        }
    }
    }
}

Primitive::Primitive(Primitive&& other) noexcept : Entity(std::move(other)) {
    type = other.type;
    bx::debugPrintf("Primitive moved: Type: %d vbh: %d ibh: %d\n", type, vbh.idx,
                    ibh.idx);
}

Primitive& Primitive::operator=(Primitive&& other) noexcept {
    if (this != &other) {
        Entity::operator=(std::move(other));
        type = other.type;
        bx::debugPrintf("Primitive moved: Type: %d vbh: %d ibh: %d\n", type,
                        vbh.idx, ibh.idx);
    }
    return *this;
}

Primitive::~Primitive() {
    bx::debugPrintf("Primitive destroyed: Type: %d vbh: %d ibh: %d\n", type,
                    vbh.idx, ibh.idx);
}

// Remember to call UpdateMesh after changing the type!
void Primitive::SetType(PrimitiveType type) { this->type = type; }

void Primitive::UpdateMesh(PhysicsCore& physicsCore,
                           bgfx::VertexLayout& layout) {
    const bgfx::Memory* verticesMem = nullptr;
    const bgfx::Memory* indicesMem = nullptr;
    GetPrimitiveTypeData(verticesMem, indicesMem, type);

    if (vbh.idx != bgfx::kInvalidHandle) {
        bgfx::destroy(vbh);
    }
    if (ibh.idx != bgfx::kInvalidHandle) {
        bgfx::destroy(ibh);
    }

    vbh = bgfx::createDynamicVertexBuffer(verticesMem, layout);
    ibh = bgfx::createIndexBuffer(indicesMem);
    if (vbh.idx == bgfx::kInvalidHandle || ibh.idx == bgfx::kInvalidHandle) {
        bx::debugPrintf("Failed to create primitive: Type: %d vbh: %d ibh: %x\n",
                        type, vbh.idx, ibh.idx);
    } else {
        bx::debugPrintf("Primitive created: Type: %d vbh: %d ibh: %d\n", type,
                        vbh.idx, ibh.idx);
    }

    // update physics
    JPH::BodyInterface& bodyInterface = physicsCore.GetBodyInterface();
    physicsCore.RemoveBody(bodyID);

    JPH::Vec3 joltPosition = ToJPH(position);
    JPH::Vec3 joltSize = ToJPH(size);
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
        bx::debugPrintf("Static Plane created: Type: %d vbh: %d ibh: %d\n", type,
                        vbh.idx, ibh.idx);

    } break;
    case RigidBodyType::Dynamic: {
        switch (type) {
        case PrimitiveType::Cube: {
            bodyID = physicsCore.AddDynamicBox(joltPosition, joltSize, 1.0f);
            bx::debugPrintf("Dynamic Cube created: Type: %d vbh: %d ibh: %d\n",
                            type, vbh.idx, ibh.idx);
        } break;
        case PrimitiveType::Sphere: {
            bodyID = physicsCore.AddDynamicSphere(size.x, joltPosition, 1.0f);
            bx::debugPrintf("Dynamic Sphere created: Type: %d vbh: %d ibh: %d\n",
                            type, vbh.idx, ibh.idx);
        } break;
        }
    }
    }
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
        PrimitiveSphere sphere;
        vertMem = bgfx::copy(sphere.vertices, sizeof(sphere.vertices));
        indiMem = bgfx::copy(sphere.indices, sizeof(sphere.indices));
    } break;
    }
    BX_ASSERT(vertMem->data != nullptr, "Vertex Memory is null");
    BX_ASSERT(vertMem->size != 0, "Vertex Memory size is 0");
    BX_ASSERT(indiMem->data != nullptr, "Index Memory is null");
    BX_ASSERT(indiMem->size != 0, "Vertex Memory size is 0");
}
