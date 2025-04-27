#include "MeshEntity.hpp"
#include "Collider.hpp"
#include "Enums.hpp"
#include "PhysicsCore.hpp"
#include "bgfx/bgfx.h"
#include "bx/debug.h"
#include "utils.hpp"
#include <utility>

MeshEntity::MeshEntity(MeshContainer& mesh, Collider* collider,
                       const RigidBodyType bodyType, PhysicsCore& physicsCore,
                       bgfx::VertexLayout& layout, Texture& texture,
                       glm::vec3 position, glm::vec3 rotation, glm::vec3 size)
    : Entity(bodyType, physicsCore, layout, texture, position, rotation, size),
      collider(collider) {
    const bgfx::Memory* verticesMem = nullptr;
    const bgfx::Memory* indicesMem = nullptr;
    mesh.GetMeshData(verticesMem, indicesMem);
    if (verticesMem == nullptr || indicesMem == nullptr) {
        bx::debugPrintf(
            "Failed to load mesh data: verticesMem: %d, indicesMem: %d",
            verticesMem, indicesMem);
        return;
    }
    vbh = bgfx::createDynamicVertexBuffer(verticesMem, layout);
    ibh = bgfx::createIndexBuffer(indicesMem, BGFX_BUFFER_INDEX32);

    SetPosition(position);
    SetRotation(rotation);
    SetSize(size);

    // setup physics
    if (bodyType == RigidBodyType::Dynamic &&
        collider->GetType() == ColliderType::Plane) {
        bx::debugPrintf("Dynamic body type can't use Plane collider.\n");
        return;
    }
    if (bodyType != RigidBodyType::Static &&
        collider->GetType() == ColliderType::Mesh) {
        bx::debugPrintf("Dynamic body type can't use Mesh collider.\n");
        return;
    }

    bodyInterface = &physicsCore.GetBodyInterface();
    JPH::Vec3 joltPosition = ToJPH(position);
    JPH::Vec3 joltSize = ToJPH(size);

    switch (bodyType) {
    case RigidBodyType::Static: {
        JPH::Vec3 colPos = ToJPH(collider->GetPosition() + position);
        bodyID = physicsCore.AddStaticCollider(colPos, collider->GetShape());
    } break;
    case RigidBodyType::Dynamic: {
        JPH::Vec3 colPos = ToJPH(collider->GetPosition() + position);
        bodyID =
            physicsCore.AddDynamicCollider(colPos, collider->GetShape(), 1.0f);
    } break;
    }
}
MeshEntity::MeshEntity(MeshEntity&& other) noexcept
    : Entity(std::move(other)), collider(other.collider), mesh(other.mesh) {}

MeshEntity& MeshEntity::operator=(MeshEntity&& other) noexcept {
    if (this != &other) {
        Entity::operator=(std::move(other));
        collider = std::move(other.collider);
        mesh = std::move(other.mesh);
    }
    return *this;
}

MeshEntity::~MeshEntity() {}

void MeshEntity::UpdateMetaData(MeshContainer* newMesh, Collider* newCollider) {
    this->mesh = newMesh;
    this->collider = newCollider;
}

void MeshEntity::UpdateMesh(PhysicsCore& physicsCore,
                            bgfx::VertexLayout& layout) {
    const bgfx::Memory* verticesMem = nullptr;
    const bgfx::Memory* indicesMem = nullptr;
    mesh->GetMeshData(verticesMem, indicesMem);
    if (verticesMem == nullptr || indicesMem == nullptr) {
        bx::debugPrintf(
            "Failed to load new mesh data: verticesMem: %d, indicesMem: %d",
            verticesMem, indicesMem);
        return;
    }
    if (vbh.idx != bgfx::kInvalidHandle) {
        bgfx::destroy(vbh);
    }
    if (ibh.idx != bgfx::kInvalidHandle) {
        bgfx::destroy(ibh);
    }
    vbh = bgfx::createDynamicVertexBuffer(verticesMem, layout);
    ibh = bgfx::createIndexBuffer(indicesMem, BGFX_BUFFER_INDEX32);

    // Update the physics body with the new mesh
    physicsCore.RemoveBody(bodyID);

    if (bodyType == RigidBodyType::Dynamic &&
        collider->GetType() == ColliderType::Plane) {
        bx::debugPrintf("Dynamic body type can't use Plane collider.\n");
        return;
    }
    if (bodyType != RigidBodyType::Static &&
        collider->GetType() == ColliderType::Mesh) {
        bx::debugPrintf("Dynamic body type can't use Mesh collider.\n");
        return;
    }

    bodyInterface = &physicsCore.GetBodyInterface();
    JPH::Vec3 joltPosition = ToJPH(position);
    JPH::Vec3 joltSize = ToJPH(size);

    switch (bodyType) {
    case RigidBodyType::Static: {
        JPH::Vec3 colPos = ToJPH(collider->GetPosition() + position);
        bodyID = physicsCore.AddStaticCollider(colPos, collider->GetShape());
    } break;
    case RigidBodyType::Dynamic: {
        JPH::Vec3 colPos = ToJPH(collider->GetPosition() + position);
        bodyID =
            physicsCore.AddDynamicCollider(colPos, collider->GetShape(), 1.0f);
    } break;
    }
}
