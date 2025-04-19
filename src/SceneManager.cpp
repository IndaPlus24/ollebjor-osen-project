#include "SceneManager.hpp"
#include <bgfx/bgfx.h>
#include <utility>
#include "Entity.hpp"
#include "MeshEntity.hpp"
#include "Primitive.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include "bx/bx.h"
#include "bx/debug.h"

static SceneManager* instance = nullptr;

SceneManager::SceneManager() {}

SceneManager::~SceneManager() {}

void SceneManager::Initialize(PhysicsCore& physicsCore,
                              bgfx::VertexLayout& layout, Renderer& renderer) {
    if (instance == nullptr)
        instance = new SceneManager();

    instance->physicsCore = &physicsCore;
    instance->layout = &layout;
    instance->renderer = &renderer;
    bx::debugPrintf("SceneManager initialized");
}

SceneManager& SceneManager::Get() {
    BX_ASSERT(instance != nullptr, "SceneManager not initialized");
    return *instance;
}

void SceneManager::Shutdown() {
    if (instance == nullptr)
        return;

    // Clean up all entities, textures, and mesh containers
    for (auto& entity : instance->entities) {
        delete entity.second;
        bx::debugPrintf("Entity removed with ID: %llu", entity.first);
    }
    for (auto& texture : instance->textures) {
        delete texture.second;
        bx::debugPrintf("Texture removed with ID: %llu", texture.first);
    }
    for (auto& mesh : instance->meshes) {
        delete mesh.second;
        bx::debugPrintf("MeshContainer removed with ID: %llu", mesh.first);
    }
    instance->entities.clear();
    instance->textures.clear();
    instance->meshes.clear();

    delete instance;
    instance = nullptr;
    bx::debugPrintf("SceneManager shutdown");
}

SceneRef<Entity> SceneManager::AddEntity(Primitive primitive) {
    uint64_t id = entities.size();
    auto entity = new Primitive(std::move(primitive));
    entities.emplace(id, entity);
    SceneRef<Entity> ref;
    ref.id = id;
    ref.data = entities.at(id);
    bx::debugPrintf("Entity added with ID: %llu", id);
    return ref;
}

SceneRef<Entity> SceneManager::AddEntity(PrimitiveType type,
                                         RigidBodyType bodyType,
                                         uint64_t textureId, glm::vec3 position,
                                         glm::vec3 rotation, glm::vec3 size) {
    // Create a new primitive and add it to the map
    uint64_t id = entities.size();
    auto texture = GetTexture(textureId);
    if (texture.data == nullptr) {
        bx::debugPrintf("Texture not found with ID: %llu", textureId);
        return {0, nullptr};
    }
    auto entity = new Primitive(type, bodyType, *physicsCore, *layout,
                                *texture.data, position, rotation, size);
    entities.emplace(id, entity);
    SceneRef<Entity> ref;
    ref.id = id;
    ref.data = entities.at(id);
    bx::debugPrintf("Primitive added with ID: %llu", id);
    return ref;
}

SceneRef<Entity> SceneManager::UpdateEntity(uint64_t id, PrimitiveType type) {
    // Check if the entity exists in the map
    auto it = entities.find(id);
    if (it != entities.end()) {
        auto entity = dynamic_cast<Primitive*>(it->second);
        if (entity == nullptr) {
            bx::debugPrintf("Entity with ID: %llu is not a Primitive", id);
            return {0, nullptr};
        }
        entity->SetType(type);
        entity->UpdateMesh(*physicsCore, *layout);
        bx::debugPrintf("Entity updated with ID: %llu", id);
        return {id, it->second};
    }
    // Return an empty reference if not found
    return {0, nullptr};
}

SceneRef<Entity> SceneManager::AddEntity(MeshEntity meshEntity) {
    // Create a new entity and add it to the map
    uint64_t id = entities.size();
    auto entity = new MeshEntity(std::move(meshEntity));
    entities.emplace(id, entity);
    SceneRef<Entity> ref;
    ref.id = id;
    ref.data = entities.at(id);
    bx::debugPrintf("MeshEntity added with ID: %llu", id);
    return ref;
}

SceneRef<Entity> SceneManager::AddEntity(uint64_t meshId, uint64_t colliderId,
                                         const RigidBodyType bodyType,
                                         uint64_t textureId, glm::vec3 position,
                                         glm::vec3 rotation, glm::vec3 size) {
    // Create a new entity and add it to the map
    uint64_t id = entities.size();
    auto mesh = GetMeshContainer(meshId);
    auto collider = GetCollider(colliderId);
    auto texture = GetTexture(textureId);
    if (mesh.data == nullptr) {
        bx::debugPrintf("Mesh not found with ID: %llu", meshId);
        return {0, nullptr};
    }
    if (collider.data == nullptr) {
        bx::debugPrintf("Collider not found with ID: %llu", colliderId);
        return {0, nullptr};
    }
    if (texture.data == nullptr) {
        bx::debugPrintf("Texture not found with ID: %llu", textureId);
        return {0, nullptr};
    }
    auto entity =
        new MeshEntity(*mesh.data, collider.data, bodyType, *physicsCore,
                       *layout, *texture.data, position, rotation, size);
    entities.emplace(id, entity);
    SceneRef<Entity> ref;
    ref.id = id;
    ref.data = entities.at(id);
    bx::debugPrintf("MeshEntity added with ID: %llu", id);
    return ref;
}

SceneRef<Entity> SceneManager::UpdateEntity(uint64_t id, uint64_t meshId,
                                            uint64_t colliderId) {
    // Check if the entity exists in the map
    auto it = entities.find(id);
    if (it != entities.end()) {
        auto mesh = GetMeshContainer(meshId);
        auto collider = GetCollider(colliderId);
        if (mesh.data == nullptr) {
            bx::debugPrintf("Mesh not found with ID: %llu", meshId);
            return {0, nullptr};
        }
        if (collider.data == nullptr) {
            bx::debugPrintf("Collider not found with ID: %llu", colliderId);
            return {0, nullptr};
        }
        auto entity = dynamic_cast<MeshEntity*>(it->second);
        if (entity == nullptr) {
            bx::debugPrintf("Entity with ID: %llu is not a MeshEntity", id);
            return {0, nullptr};
        }
        entity->UpdateMetaData(mesh.data, collider.data);
        entity->UpdateMesh(*physicsCore, *layout);
        bx::debugPrintf("Entity updated with ID: %llu", id);
        return {id, it->second};
    }
    // Return an empty reference if not found
    return {0, nullptr};
}

SceneRef<Entity> SceneManager::GetEntity(const uint64_t id) {
    // Check if the entity exists in the map
    auto it = entities.find(id);
    if (it != entities.end()) {
        SceneRef<Entity> ref;
        ref.id = id;
        ref.data = it->second;
        bx::debugPrintf("Entity found with ID: %llu", id);
        return ref;
    }
    // Return an empty reference if not found
    return {0, nullptr};
}

void SceneManager::RemoveEntity(const uint64_t id) {
    // Check if the entity exists in the map
    auto it = entities.find(id);
    if (it != entities.end()) {
        delete it->second;
        entities.erase(it);
        bx::debugPrintf("Entity removed with ID: %llu", id);
    } else {
        bx::debugPrintf("Entity not found with ID: %llu", id);
    }
}

SceneRef<Texture> SceneManager::AddTexture(Texture texture) {
    // Create a new texture and add it to the map
    uint64_t id = textures.size();
    auto texturePtr = new Texture(std::move(texture));
    textures.emplace(id, texturePtr);
    SceneRef<Texture> ref;
    ref.id = id;
    ref.data = textures.at(id);
    bx::debugPrintf("Texture added with ID: %llu", id);
    return ref;
}

SceneRef<Texture> SceneManager::AddTexture(const std::string& filePath,
                                           uint32_t flags) {
    // Create a new texture and add it to the map
    uint64_t id = textures.size();
    auto texturePtr = new Texture(filePath, flags);
    textures.emplace(id, texturePtr);
    SceneRef<Texture> ref;
    ref.id = id;
    ref.data = textures.at(id);
    bx::debugPrintf("Texture added with ID: %llu", id);
    return ref;
}

SceneRef<Texture> SceneManager::GetTexture(const uint64_t id) {
    // Check if the texture exists in the map
    auto it = textures.find(id);
    if (it != textures.end()) {
        SceneRef<Texture> ref;
        ref.id = id;
        ref.data = it->second;
        return ref;
    }
    // Return an empty reference if not found
    return {0, nullptr};
}

void SceneManager::RemoveTexture(const uint64_t id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        delete it->second;
        textures.erase(it);
        bx::debugPrintf("Texture removed with ID: %llu", id);
    } else {
        bx::debugPrintf("Texture not found with ID: %llu", id);
    }
}

SceneRef<MeshContainer>
SceneManager::AddMeshContainer(MeshContainer meshContainer) {
    // Create a new mesh container and add it to the map
    uint64_t id = meshes.size();
    auto meshPtr = new MeshContainer(std::move(meshContainer));
    meshes.emplace(id, meshPtr);
    SceneRef<MeshContainer> ref;
    ref.id = id;
    ref.data = meshes.at(id);
    bx::debugPrintf("MeshContainer added with ID: %llu", id);
    return ref;
}

SceneRef<MeshContainer>
SceneManager::AddMeshContainer(const std::string& path) {
    // Create a new mesh container and add it to the map
    uint64_t id = meshes.size();
    auto meshPtr = new MeshContainer(path);
    meshes.emplace(id, meshPtr);
    SceneRef<MeshContainer> ref;
    ref.id = id;
    ref.data = meshes.at(id);
    bx::debugPrintf("MeshContainer added with ID: %llu", id);
    return ref;
}

SceneRef<MeshContainer> SceneManager::GetMeshContainer(const uint64_t id) {
    // Check if the mesh container exists in the map
    auto it = meshes.find(id);
    if (it != meshes.end()) {
        SceneRef<MeshContainer> ref;
        ref.id = id;
        ref.data = it->second;
        return ref;
    }
    // Return an empty reference if not found
    return {0, nullptr};
}

void SceneManager::RemoveMeshContainer(const uint64_t id) {
    auto it = meshes.find(id);
    if (it != meshes.end()) {
        delete it->second;
        meshes.erase(it);
        bx::debugPrintf("MeshContainer removed with ID: %llu", id);
    } else {
        bx::debugPrintf("MeshContainer not found with ID: %llu", id);
    }
}

SceneRef<Collider> SceneManager::AddCollider(Collider collider) {
    // Create a new collider and add it to the map
    uint64_t id = colliders.size();
    auto colliderPtr = new Collider(std::move(collider));
    colliders.emplace(id, colliderPtr);
    SceneRef<Collider> ref;
    ref.id = id;
    ref.data = colliders.at(id);
    bx::debugPrintf("Collider added with ID: %llu", id);
    return ref;
}

SceneRef<Collider> SceneManager::AddCollider(ColliderType type,
                                             const glm::vec3& position,
                                             const glm::vec3& rotation,
                                             const glm::vec3& size) {
    // Create a new collider and add it to the map
    uint64_t id = colliders.size();
    auto colliderPtr = new Collider(type, position, rotation, size);
    colliders.emplace(id, colliderPtr);
    SceneRef<Collider> ref;
    ref.id = id;
    ref.data = colliders.at(id);
    bx::debugPrintf("Collider added with ID: %llu", id);
    return ref;
}

SceneRef<Collider> SceneManager::GetCollider(const uint64_t id) {
    // Check if the collider exists in the map
    auto it = colliders.find(id);
    if (it != colliders.end()) {
        SceneRef<Collider> ref;
        ref.id = id;
        ref.data = it->second;
        return ref;
    }
    // Return an empty reference if not found
    return {0, nullptr};
}

void SceneManager::RemoveCollider(const uint64_t id) {
    auto it = colliders.find(id);
    if (it != colliders.end()) {
        delete it->second;
        colliders.erase(id);
        bx::debugPrintf("Collider removed with ID: %llu", id);
    } else {
        bx::debugPrintf("Collider not found with ID: %llu", id);
    }
}

SceneRef<Camera> SceneManager::AddCamera(Camera camera) {
    // Create a new camera and add it to the map
    uint64_t id = cameras.size();
    auto cameraPtr = new Camera(std::move(camera));
    cameras.emplace(id, cameraPtr);
    SceneRef<Camera> ref;
    ref.id = id;
    ref.data = cameras.at(id);
    bx::debugPrintf("Camera added with ID: %llu", id);
    return ref;
}

SceneRef<Camera> SceneManager::AddCamera(const glm::vec3& position,
                                         const glm::vec3& up, const float fov,
                                         const float nearPlane,
                                         const float farPlane) {
    // Create a new camera and add it to the map
    uint64_t id = cameras.size();
    auto cameraPtr =
        new Camera(*renderer, position, up, fov, nearPlane, farPlane);
    cameras.emplace(id, cameraPtr);
    SceneRef<Camera> ref;
    ref.id = id;
    ref.data = cameras.at(id);
    bx::debugPrintf("Camera added with ID: %llu", id);
    return ref;
}

SceneRef<Camera> SceneManager::GetCamera(const uint64_t id) {
    // Check if the camera exists in the map
    auto it = cameras.find(id);
    if (it != cameras.end()) {
        SceneRef<Camera> ref;
        ref.id = id;
        ref.data = it->second;
        return ref;
    }
    // Return an empty reference if not found
    return {0, nullptr};
}

void SceneManager::RemoveCamera(const uint64_t id) {
    // Check if the camera exists in the map
    auto it = cameras.find(id);
    if (it != cameras.end()) {
        delete it->second;
        cameras.erase(it);
        bx::debugPrintf("Camera removed with ID: %llu", id);
    } else {
        bx::debugPrintf("Camera not found with ID: %llu", id);
    }
}

SceneRef<Camera> SceneManager::GetActiveCamera() {
    // Check if the active camera exists in the map
    auto it = cameras.find(activeCameraId);
    if (it != cameras.end()) {
        SceneRef<Camera> ref;
        ref.id = activeCameraId;
        ref.data = it->second;
        return ref;
    }
    // Return an empty reference if not found
    return {0, nullptr};
}