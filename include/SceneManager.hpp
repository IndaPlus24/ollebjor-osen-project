#pragma once

#include "Collider.hpp"
#include "Entity.hpp"
#include "MeshContainer.hpp"
#include "MeshEntity.hpp"
#include "Primitive.hpp"
#include "Texture.hpp"
#include <unordered_map>

template <typename T> struct SceneRef {
    uint64_t id;
    T* data;
};

class SceneManager {
  public:
    SceneManager();
    ~SceneManager();

    static void Initialize();
    static SceneManager& GetInstance();
    static void Shutdown();

    SceneRef<Entity> addEntity(Primitive primitive);
    SceneRef<Entity> addEntity(MeshEntity meshEntity);
    SceneRef<Entity> getEntity(const uint64_t id);
    void removeEntity(const uint64_t id);

    SceneRef<Texture> addTexture(Texture texture);
    SceneRef<Texture> getTexture(const uint64_t id);
    void removeTexture(const uint64_t id);

    SceneRef<MeshContainer> addMeshContainer(MeshContainer meshContainer);
    SceneRef<MeshContainer> getMeshContainer(const uint64_t id);
    void removeMeshContainer(const uint64_t id);

    SceneRef<Collider> addCollider(Collider collider);
    SceneRef<Collider> getCollider(const uint64_t id);
    void removeCollider(const uint64_t id);

    inline std::unordered_map<uint64_t, Entity*>& getEntities() {
        return entities;
    }

    inline std::unordered_map<uint64_t, Texture*>& getTextures() {
        return textures;
    }

    inline std::unordered_map<uint64_t, MeshContainer*>& getMeshContainers() {
        return meshes;
    }

    inline std::unordered_map<uint64_t, Collider*>& getColliders() {
        return colliders;
    }

  private:
    std::unordered_map<uint64_t, Entity*> entities;
    std::unordered_map<uint64_t, Texture*> textures;
    std::unordered_map<uint64_t, MeshContainer*> meshes;
    std::unordered_map<uint64_t, Collider*> colliders;
};
