#pragma once

#include "Camera.hpp"
#include "Collider.hpp"
#include "Entity.hpp"
#include "Enums.hpp"
#include "Material.hpp"
#include "MeshContainer.hpp"
#include "MeshEntity.hpp"
#include "Primitive.hpp"
#include "Renderer.hpp"
#include "Texture.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>

// Forward declaration
class SceneImporter;

template <typename T> struct SceneRef {
    uint64_t id;
    T* data;
};

class SceneManager {
  private:
    std::unordered_map<uint64_t, Entity*> entities;
    std::unordered_map<uint64_t, Texture*> textures;
    std::unordered_map<uint64_t, MeshContainer*> meshes;
    std::unordered_map<uint64_t, Collider*> colliders;
    std::unordered_map<uint64_t, Camera*> cameras;
    std::unordered_map<uint64_t, Material*> materials;
    std::unordered_map<std::string, uint32_t> loadedURIs;


    SceneImporter* sceneImporter;
    PhysicsCore* physicsCore;
    bgfx::VertexLayout* layout;
    Renderer* renderer;

    uint64_t activeCameraId = 0;

  public:
    SceneManager();
    ~SceneManager();

    static void Initialize(PhysicsCore& physicsCore, bgfx::VertexLayout& layout,
                           Renderer& renderer, SceneImporter& sceneImporter);
    static SceneManager& Get();
    static void Shutdown();

    SceneRef<Entity> AddEntity(Primitive primitive);
    SceneRef<Entity> AddEntity(PrimitiveType type, RigidBodyType bodyType,
                               uint64_t materialId,
                               glm::vec3 position = glm::vec3(0.0f),
                               glm::vec3 rotation = glm::vec3(0.0f),
                               glm::vec3 size = glm::vec3(1.0f));
    SceneRef<Entity> UpdateEntity(uint64_t id, PrimitiveType type);

    SceneRef<Entity> AddEntity(MeshEntity meshEntity);
    SceneRef<Entity> AddEntity(uint64_t meshId, uint64_t colliderId,
                               const RigidBodyType bodyType,
                               uint64_t materialId,
                               glm::vec3 position = glm::vec3(0.0f),
                               glm::vec3 rotation = glm::vec3(0.0f),
                               glm::vec3 size = glm::vec3(1.0f));
    SceneRef<Entity> UpdateEntity(uint64_t id, uint64_t meshId,
                                  uint64_t colliderId);

    SceneRef<Entity> GetEntity(const uint64_t id);
    void RemoveEntity(const uint64_t id);

    std::vector<SceneRef<Entity>> AddScene(const std::string& path);

    SceneRef<Texture> AddTexture(Texture texture);
    SceneRef<Texture> AddTexture(const std::string& filePath,
                                 uint32_t flags = 0);

    SceneRef<Texture> GetTexture(const uint64_t id);
    void RemoveTexture(const uint64_t id);

    SceneRef<MeshContainer> AddMeshContainer(MeshContainer meshContainer);
    SceneRef<MeshContainer> AddMeshContainer(const std::string& path);
    SceneRef<MeshContainer> AddMeshContainer(std::string path, std::vector<Vertex> vertices,
                                            std::vector<uint32_t> indices);

    SceneRef<MeshContainer> GetMeshContainer(const uint64_t id);
    void RemoveMeshContainer(const uint64_t id);

    SceneRef<Collider> AddCollider(Collider collider);
    SceneRef<Collider> AddCollider(ColliderType type,
                                   const glm::vec3& position = glm::vec3(0.0f),
                                   const glm::vec3& rotation = glm::vec3(0.0f),
                                   const glm::vec3& size = glm::vec3(1.0f));

    SceneRef<Collider> GetCollider(const uint64_t id);
    void RemoveCollider(const uint64_t id);

    SceneRef<Material> AddMaterial(Material material);
    SceneRef<Material> AddMaterial(uint64_t albedoId, uint64_t normalId);
    SceneRef<Material> AddMaterial(std::string albedoPath,
                                   std::string normalPath);

    SceneRef<Material> GetMaterial(const uint64_t id);
    void RemoveMaterial(const uint64_t id);

    SceneRef<Camera> AddCamera(Camera camera);
    SceneRef<Camera>
    AddCamera(const glm::vec3& position = glm::vec3(0.0f),
              const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
              const float fov = 60.0f, const float nearPlane = 0.1f,
              const float farPlane = 100.0f);
    SceneRef<Camera> GetCamera(const uint64_t id);

    void RemoveCamera(const uint64_t id);
    SceneRef<Camera> GetActiveCamera();

    Renderer& GetRenderer() { return *renderer; }
    inline void SetActiveCamera(const uint64_t id) { activeCameraId = id; }

    inline std::unordered_map<uint64_t, Entity*>& GetEntities() {
        return entities;
    }

    inline std::unordered_map<uint64_t, Texture*>& GetTextures() {
        return textures;
    }

    inline std::unordered_map<uint64_t, MeshContainer*>& GetMeshContainers() {
        return meshes;
    }

    inline std::unordered_map<uint64_t, Collider*>& GetColliders() {
        return colliders;
    }

    inline void SetPhysicsCore(PhysicsCore& physicsCore) {
        this->physicsCore = &physicsCore;
    }
    inline void SetLayout(bgfx::VertexLayout& layout) {
        this->layout = &layout;
    }
};
