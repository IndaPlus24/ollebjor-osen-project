#include "Primitive.hpp"
#include "Entity.hpp"
#include "Jolt/Math/Vec3.h"
#include "Jolt/Physics/Body/BodyID.h"
#include "PhysicsCore.hpp"
#include "PrimitiveDefinitions.hpp"
#include "Enums.hpp"
#include "Texture.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bx/debug.h"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "utils.hpp"
#include "GameEngineLogger.hpp"
#include <cstddef>
#include <format>

Primitive::Primitive(PrimitiveType type, RigidBodyType bodyType,
                     PhysicsCore& physicsCore, bgfx::VertexLayout& layout,
                     Texture& texture, glm::vec3 position, glm::vec3 rotation,
                     glm::vec3 size)
    : Entity(bodyType, physicsCore, layout, texture, position, rotation, size) {
    const bgfx::Memory* verticesMem = nullptr;
    const bgfx::Memory* indicesMem = nullptr;
    GetPrimitiveTypeData(verticesMem, indicesMem, type);

    vbh = bgfx::createDynamicVertexBuffer(verticesMem, layout);
    ibh = bgfx::createIndexBuffer(indicesMem);
    if (vbh.idx == bgfx::kInvalidHandle || ibh.idx == bgfx::kInvalidHandle) {
        LOG_ERROR("Renderer",
                  std::format(
                      "Failed to create primitive: Type: {}, vbh: {}, ibh: {}",
                      static_cast<int>(type), vbh.idx, ibh.idx));
    } else {
        LOG_INFO("Renderer",
                 "Primitive created: Type: " + std::to_string(static_cast<int>(type)) +
                     " vbh: " + std::to_string(vbh.idx) +
                     " ibh: " + std::to_string(ibh.idx));
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
        LOG_INFO("Physics",
                 "Static Plane created: Type: " + std::to_string(static_cast<int>(type)) +
                     " vbh: " + std::to_string(vbh.idx) +
                     " ibh: " + std::to_string(ibh.idx));

    } break;
    case RigidBodyType::Dynamic: {
        switch (type) {
        case PrimitiveType::Cube: {
            bodyID = physicsCore.AddDynamicBox(joltPosition, joltSize, 1.0f);
            LOG_INFO("Physics",
                     "Dynamic Cube created: Type: " + std::to_string(static_cast<int>(type)) +
                         " vbh: " + std::to_string(vbh.idx) +
                         " ibh: " + std::to_string(ibh.idx));
        } break;
        case PrimitiveType::Sphere: {
            bodyID = physicsCore.AddDynamicSphere(size.x, joltPosition, 1.0f);
            LOG_INFO("Physics",
                     "Dynamic Sphere created: Type: " + std::to_string(static_cast<int>(type)) +
                         " vbh: " + std::to_string(vbh.idx) +
                         " ibh: " + std::to_string(ibh.idx));
        } break;
        }
    }
    }
}

Primitive::Primitive(Primitive&& other) noexcept : Entity(std::move(other)) {
    type = other.type;
    LOG_DEBUG("Entity", "Primitive moved: Type: " + std::to_string(static_cast<int>(type)) +
                            " vbh: " + std::to_string(vbh.idx) +
                            " ibh: " + std::to_string(ibh.idx));
}

Primitive::~Primitive() {
    LOG_DEBUG("Entity", "Primitive destroyed: Type: " + std::to_string(static_cast<int>(type)) +
                            " vbh: " + std::to_string(vbh.idx) +
                            " ibh: " + std::to_string(ibh.idx));
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
