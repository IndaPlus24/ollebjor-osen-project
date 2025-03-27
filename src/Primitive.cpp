#include "Primitive.hpp"
#include "PrimitiveDefinitions.hpp"
#include "Enums.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"
#include <iostream>

Primitive::Primitive(PrimitiveType type, bgfx::VertexLayout& layout,
                     glm::vec3 position, glm::vec3 rotation, glm::vec3 size)
    : type(type), position(position), rotation(rotation), size(size) {
    GetPrimitiveTypeData(verticesMem, indicesMem, type);

    vbh = bgfx::createVertexBuffer(verticesMem, layout);
    ibh = bgfx::createIndexBuffer(indicesMem);
    if (vbh.idx == bgfx::kInvalidHandle || ibh.idx == bgfx::kInvalidHandle) {
        std::cout << "Failed to create primitive: Type: " << (int)type
                  << " vbh: " << vbh.idx << " ibh: " << ibh.idx << std::endl;
    } else {
        std::cout << "Primitive created: Type: " << (int)type
                  << " vbh: " << vbh.idx << " ibh: " << ibh.idx << std::endl;
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
        std::cout << "Failed to create primitive: Type: " << (int)type
                  << " vbh: " << vbh.idx << " ibh: " << ibh.idx << std::endl;
    } else {
        std::cout << "Primitive created: Type: " << (int)type
                  << " vbh: " << vbh.idx << " ibh: " << ibh.idx << std::endl;
    }

    SetPosition(position);
    SetRotation(rotation);
    SetSize(size);
}

Primitive::Primitive(Primitive&& other) noexcept {
    type = other.type;
    position = other.position;
    rotation = other.rotation;
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
    std::cout << "Primitive moved: Type: " << (int)type << " vbh: " << vbh.idx
              << " ibh: " << ibh.idx << std::endl;
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
        std::cout << "Empty Primitive destroy: Type: " << (int)type
                  << " vbh: " << vbh.idx << " ibh: " << ibh.idx << std::endl;
        return;
    }
    std::cout << "Primitive destroyed: Type: " << (int)type
              << " vbh: " << vbh.idx << " ibh: " << ibh.idx << std::endl;
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
                                     PrimitiveType type) {
    switch (type) {
    case PrimitiveType::Cube:
        vertMem = bgfx::makeRef(cube_vertices, 8 * sizeof(Vertex));
        indiMem = bgfx::makeRef(cube_indices, 36 * sizeof(uint16_t));
        break;
    case PrimitiveType::Plane:
        vertMem = bgfx::makeRef(quad_vertices, 4 * sizeof(Vertex));
        indiMem = bgfx::makeRef(quad_indices, 6 * sizeof(uint16_t));
        break;
    case PrimitiveType::Sphere:
        vertMem = bgfx::makeRef(icoshere_vertices, 12 * sizeof(Vertex));
        indiMem = bgfx::makeRef(icoshere_indices, 60 * sizeof(uint16_t));
        break;
    }
    BX_ASSERT(vertMem->data != nullptr, "Vertex Memory is null");
    BX_ASSERT(vertMem->size != 0, "Vertex Memory size is 0");
    BX_ASSERT(indiMem->data != nullptr, "Index Memory is null");
    BX_ASSERT(indiMem->size != 0, "Vertex Memory size is 0");
}
