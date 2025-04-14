#pragma once

#include "Enums.hpp"
#include <Jolt/Jolt.h>
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Vertex.hpp"
#include <glm/glm.hpp>

class Collider {
  private:
    ColliderType type;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 size;
    JPH::Ref<JPH::Shape> shape;

  public:
    Collider(ColliderType type, const glm::vec3& position,
             const glm::vec3& rotation, const glm::vec3& size);
    Collider(ColliderType type, const glm::vec3& position,
             const glm::vec3& rotation, const glm::vec3& size,
             const std::vector<Vertex>& vertices,
             const std::vector<uint32_t>& indices);
    Collider(Collider&& other) noexcept;
    Collider& operator=(Collider&& other) noexcept;
    ~Collider();

    inline ColliderType GetType() const { return type; }
    inline const glm::vec3& GetPosition() const { return position; }
    inline const glm::vec3& GetRotation() const { return rotation; }
    inline const glm::vec3& GetSize() const { return size; }
    inline const JPH::Ref<JPH::Shape>& GetShape() const { return shape; }
};
