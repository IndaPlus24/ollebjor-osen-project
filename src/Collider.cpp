#include "Collider.hpp"
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/PlaneShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include "Jolt/Geometry/IndexedTriangle.h"
#include "Jolt/Geometry/Triangle.h"
#include "Jolt/Math/Float3.h"
#include "Jolt/Math/Vec3.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"
#include "Vertex.hpp"
#include "bx/debug.h"
#include "utils.hpp"

Collider::Collider(ColliderType type, const glm::vec3& position,
                   const glm::vec3& rotation, const glm::vec3& size)
    : type(type), position(position), rotation(rotation), size(size) {
    switch (type) {
    case ColliderType::Box: {
        const JPH::Vec3 joltSize = ToJPH(size);
        shape = new JPH::BoxShape(joltSize);
    } break;
    case ColliderType::Sphere: {
        shape = new JPH::SphereShape(size.x);
    } break;
    case ColliderType::Capsule: {
        shape = new JPH::CapsuleShape(size.x, size.y);
    } break;
    case ColliderType::Plane: {
        const JPH::Vec3 joltNormal = ToJPH(rotation);
        JPH::Plane planeSettinge(joltNormal, 0);
        shape = new JPH::PlaneShape(planeSettinge);
    } break;
    default:
        break;
    }
}

Collider::Collider(ColliderType type, const glm::vec3& position,
                   const glm::vec3& rotation, const glm::vec3& size,
                   const std::vector<Vertex>& vertices,
                   const std::vector<uint32_t>& indices)
    : type(type), position(position), rotation(rotation), size(size) {
    if (type != ColliderType::Mesh) {
        bx::debugPrintf(
            "Error: Must use Mesh collider with this constructor.\n");
        return;
    }
    const JPH::Vec3 joltSize = ToJPH(size);
    JPH::VertexList verticesList;
    JPH::IndexedTriangleList indicesList;

    if (vertices.size() != 0 && indices.size() != 0) {
        bx::debugPrintf("Creating mesh collider\n");
        for (const auto& vertex : vertices) {
            JPH::Float3 joltVertex = {vertex.pos.x, vertex.pos.y, vertex.pos.z};
            verticesList.push_back(joltVertex);
        }
        for (size_t i = 0; i < indices.size(); i += 3) {
            JPH::IndexedTriangle joltIndex = {indices[i], indices[i + 1],
                                              indices[i + 2]};
            indicesList.push_back(joltIndex);
        }
    } else {
        // Handle the case where vertices or indices are null
        // You can either throw an exception or create a default mesh
        // For now, we'll just log a message
        bx::debugPrintf(
            "Error: Vertices or indices are null for mesh collider\n");
        return;
    }

    bx::debugPrintf("Vertices size: %zu\n", verticesList.size());
    JPH::MeshShapeSettings meshSettings(verticesList, indicesList);
    JPH::ShapeSettings::ShapeResult result;
    shape = new JPH::MeshShape(meshSettings, result);
}

Collider::Collider(Collider&& other) noexcept
    : type(other.type), position(other.position), rotation(other.rotation),
      size(other.size), shape(std::move(other.shape)) {
    other.shape = nullptr;
}

Collider& Collider::operator=(Collider&& other) noexcept {
    if (this != &other) {
        type = other.type;
        position = other.position;
        rotation = other.rotation;
        size = other.size;
        shape = std::move(other.shape);
        other.shape = nullptr;
    }
    return *this;
}

Collider::~Collider() {
    if (shape != nullptr) {
        bx::debugPrintf("Collider destructor\n");
        shape->Release();
        shape = nullptr;
    }
}
