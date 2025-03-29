#include "Primitive.hpp"
#include "Jolt/Math/Vec3.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "PrimitiveDefinitions.hpp"
#include "Enums.hpp"
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bx/debug.h"
#include "glm/fwd.hpp"
#include "glm/gtc/quaternion.hpp"

Primitive::Primitive(PrimitiveType type, bgfx::VertexLayout& layout,
                     JPH::BodyInterface& BodyInterface, uint32_t abgr,
                     glm::vec3 position, glm::vec3 rotation, glm::vec3 size)
    : type(type), abgr(abgr), position(position), rotation(rotation),
      size(size) {
    const bgfx::Memory* verticesMem = nullptr;
    const bgfx::Memory* indicesMem = nullptr;
    GetPrimitiveTypeData(verticesMem, indicesMem, type, abgr);

    vbh = bgfx::createDynamicVertexBuffer(verticesMem, layout);
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

    shape = new JPH::SphereShape(size.x);

    JPH::Vec3 jPosition(position.x, position.y, position.z);
    JPH::BodyCreationSettings bodySettings(
        shape, jPosition, JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, 0);

    bodyID = BodyInterface.CreateAndAddBody(bodySettings, JPH::EActivation::Activate);
}

Primitive::Primitive(bgfx::VertexLayout& layout)
    : type(PrimitiveType::Cube), position(glm::vec3(0.0f)),
      rotation(glm::vec3(0.0f)), size(glm::vec3(1.0f)) {
    const bgfx::Memory* verticesMem = nullptr;
    const bgfx::Memory* indicesMem = nullptr;
    GetPrimitiveTypeData(verticesMem, indicesMem, PrimitiveType::Cube);

    vbh = bgfx::createDynamicVertexBuffer(verticesMem, layout);
    ibh = bgfx::createIndexBuffer(indicesMem);
    if (vbh.idx == bgfx::kInvalidHandle || ibh.idx == bgfx::kInvalidHandle) {
        bx::debugPrintf("Failed to create primitive: Type: %d vbh: %d ibh: %x",
                        type, vbh.idx, ibh.idx);
    } else {
        bx::debugPrintf("Primitive created: Type: %d vbh: %d ibh: %d Color: %x",
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
    transform = other.transform;
    bodyID = other.bodyID;
    other.vbh.idx = bgfx::kInvalidHandle;
    other.ibh.idx = bgfx::kInvalidHandle;
    other.bodyID = JPH::BodyID();
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

void Primitive::SetColor(uint32_t abgr) {
    if (abgr == this->abgr) {
        return;
    }
    bx::debugPrintf(
        "Primitive color changed: Type: %d vbh: %d ibh: %d Color: %x\n", type,
        vbh.idx, ibh.idx, abgr);
    const bgfx::Memory* verticesMem = nullptr;
    switch (type) {
    case PrimitiveType::Cube: {
        PrimitiveCube cube(abgr);
        verticesMem = bgfx::copy(cube.vertices, sizeof(cube.vertices));
    } break;
    case PrimitiveType::Plane: {
        PrimitiveQuad quad(abgr);
        verticesMem = bgfx::copy(quad.vertices, sizeof(quad.vertices));
    } break;
    case PrimitiveType::Sphere: {
        PrimitiveIcosphere sphere(abgr);
        verticesMem = bgfx::copy(sphere.vertices, sizeof(sphere.vertices));
    } break;
    default:
        return;
    }
    this->abgr = abgr;
    bgfx::update(vbh, 0, verticesMem);
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
        vertMem = bgfx::copy(cube.vertices, sizeof(cube.vertices));
        indiMem = bgfx::copy(cube.indices, sizeof(cube.indices));
    } break;
    case PrimitiveType::Plane: {
        PrimitiveQuad quad(abgr);
        vertMem = bgfx::copy(quad.vertices, sizeof(quad.vertices));
        indiMem = bgfx::copy(quad.indices, sizeof(quad.indices));
    } break;
    case PrimitiveType::Sphere: {
        PrimitiveIcosphere sphere(abgr);
        vertMem = bgfx::copy(sphere.vertices, sizeof(sphere.vertices));
        indiMem = bgfx::copy(sphere.indices, sizeof(sphere.indices));
    } break;
    }
    BX_ASSERT(vertMem->data != nullptr, "Vertex Memory is null");
    BX_ASSERT(vertMem->size != 0, "Vertex Memory size is 0");
    BX_ASSERT(indiMem->data != nullptr, "Index Memory is null");
    BX_ASSERT(indiMem->size != 0, "Vertex Memory size is 0");
}
