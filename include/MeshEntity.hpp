#pragma once

#include <Jolt/Jolt.h>
#include "Collider.hpp"
#include "Entity.hpp"
#include "Enums.hpp"
#include "MeshContainer.hpp"
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>

class MeshEntity : public Entity {
  private:
    Collider* collider;
    MeshContainer* mesh;

  public:
    MeshEntity(MeshContainer& mesh, Collider* collider,
               const RigidBodyType bodyType, PhysicsCore& physicsCore,
               bgfx::VertexLayout& layout, uint64_t materialId,
               glm::vec3 position = glm::vec3(0.0f),
               glm::vec3 rotation = glm::vec3(0.0f),
               glm::vec3 size = glm::vec3(1.0f));
    MeshEntity(MeshEntity&& other) noexcept;
    MeshEntity& operator=(MeshEntity&& other) noexcept;
    MeshEntity(const MeshEntity&) = delete;
    MeshEntity& operator=(const MeshEntity&) = delete;
    ~MeshEntity();

    void UpdateMetaData(MeshContainer* newMesh, Collider* newCollider);
    void UpdateMesh(PhysicsCore& physicsCore, bgfx::VertexLayout& layout) override;
};
