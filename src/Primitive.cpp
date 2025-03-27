#include "Primitive.hpp"
#include "PrimitiveDefinitions.hpp"
#include "Enums.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include <iostream>

Primitive::Primitive(PrimitiveType type, bgfx::VertexLayout& layout,
                     uint32_t abgr, glm::vec3 position, glm::vec3 rotation,
                     glm::vec3 size)
    : type(type), abgr(abgr), position(position), rotation(rotation),
      size(size) {
    GetPrimitiveTypeData(verticesMem, indicesMem, type, abgr);

    vbh = bgfx::createVertexBuffer(verticesMem, layout);
    ibh = bgfx::createIndexBuffer(indicesMem);
    if (vbh.idx == bgfx::kInvalidHandle || ibh.idx == bgfx::kInvalidHandle) {
        bx::debugPrintf("Failed to create primitive: Type: %d vbh: %d ibh: %d",
                        type, vbh.idx, ibh.idx);
    } else {
        bx::debugPrintf("Primitive created: Type: %d vbh: %d ibh: %d Color: %d",
                        type, vbh.idx, ibh.idx, abgr);
    }

    SetPosition(position);
    SetRotation(rotation);
    SetSize(size);
}

Primitive::Primitive(bgfx::VertexLayout& layout)
    : type(PrimitiveType::Cube), position(glm::vec3(0.0f)),
      rotation(glm::vec3(0.0f)), size(glm::vec3(1.0f)) {
    GetPrimitiveTypeData(verticesMem, indicesMem, PrimitiveType::Cube);

    vbh = bgfx::createVertexBuffer(verticesMem, layout);
    ibh = bgfx::createIndexBuffer(indicesMem);
    if (vbh.idx == bgfx::kInvalidHandle || ibh.idx == bgfx::kInvalidHandle) {
        bx::debugPrintf("Failed to create primitive: Type: %d vbh: %d ibh: %d",
                        type, vbh.idx, ibh.idx);
    } else {
        bx::debugPrintf("Primitive created: Type: %d vbh: %d ibh: %d Color: %d",
                        type, vbh.idx, ibh.idx, abgr);
    }

    SetPosition(position);
    SetRotation(rotation);
    SetSize(size);
}

Primitive::Primitive(Primitive&& other) noexcept {
    type = other.type;
    position = other.position;
    rotation = other.rotation;
    abgr = other.abgr;
    size = other.size;
    vbh = other.vbh;
    ibh = other.ibh;
    verticesMem = other.verticesMem;
    indicesMem = other.indicesMem;
    transform = other.transform;
    other.vbh.idx = bgfx::kInvalidHandle;
    other.ibh.idx = bgfx::kInvalidHandle;
    other.verticesMem = nullptr;
    other.indicesMem = nullptr;
    bx::debugPrintf("Primitive moved: Type: %d vbh: %d ibh: %d", type, vbh.idx,
                    ibh.idx);
}

Primitive::~Primitive() {
    bool isInvalidVBH = vbh.idx == bgfx::kInvalidHandle;
    bool isInvalidIBH = ibh.idx == bgfx::kInvalidHandle;
    if (!isInvalidVBH) {
        bgfx::destroy(vbh);
    }
    if (!isInvalidIBH) {
        bgfx::destroy(ibh);
    }
    if (isInvalidIBH || isInvalidVBH) {
        bx::debugPrintf("Empty Primitive destroy: Type: %d vbh: %d ibh: %d",
                        type, vbh.idx, ibh.idx);
        return;
    }
    bx::debugPrintf("Primitive destroyed: Type: %d vbh: %d ibh: %d", type,
                    vbh.idx, ibh.idx);
}

// From: https://stackoverflow.com/a/66054048
// Now with the quaternion transform you rotate any vector or compound it with
// another transformation matrix
void QuaternionRotate(glm::mat4& result, const glm::vec3& axis, float angle) {
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
                                     PrimitiveType type, uint32_t abgr) {
    switch (type) {
    case PrimitiveType::Cube: {
        PrimitiveCube cube(abgr);
        vertMem = bgfx::alloc(8 * sizeof(Vertex));
        indiMem = bgfx::alloc(36 * sizeof(uint16_t));
        memcpy(vertMem->data, cube.vertices, 8 * sizeof(Vertex));
        memcpy(indiMem->data, cube.indices, 36 * sizeof(uint16_t));
    } break;
    case PrimitiveType::Plane: {
        PrimitiveQuad quad(abgr);
        vertMem = bgfx::alloc(4 * sizeof(Vertex));
        indiMem = bgfx::alloc(6 * sizeof(uint16_t));
        memcpy(vertMem->data, quad.vertices, 4 * sizeof(Vertex));
        memcpy(indiMem->data, quad.indices, 6 * sizeof(uint16_t));
    } break;
    case PrimitiveType::Sphere: {
        PrimitiveIcosphere sphere(abgr);
        vertMem = bgfx::alloc(12 * sizeof(Vertex));
        indiMem = bgfx::alloc(59 * sizeof(uint16_t));
        memcpy(vertMem->data, sphere.vertices, 12 * sizeof(Vertex));
        memcpy(indiMem->data, sphere.indices, 59 * sizeof(uint16_t));
    } break;
    }
    BX_ASSERT(vertMem->data != nullptr, "Vertex Memory is null");
    BX_ASSERT(vertMem->size != 0, "Vertex Memory size is 0");
    BX_ASSERT(indiMem->data != nullptr, "Index Memory is null");
    BX_ASSERT(indiMem->size != 0, "Vertex Memory size is 0");
}
