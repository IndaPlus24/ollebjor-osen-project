#pragma once

#include "SceneManager.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <cstdint>
#include <string>
#include <vector>

class SceneImporter {
  private:
    SceneManager* sceneManager;
    std::string workingDirectory;
    std::vector<SceneRef<Entity>> sceneRefs;

    void processNode(aiNode* node, const aiScene* scene);
    SceneRef<MeshContainer> processMesh(aiMesh* mesh, const aiScene* scene,
                                        uint64_t& matId);
    uint64_t loadMaterialTextures(aiMaterial* mat, aiTextureType type);

  public:
    SceneImporter() = default;
    ~SceneImporter() = default;

    std::vector<SceneRef<Entity>> ImportScene(const std::string& path);

    inline void SetSceneManager(SceneManager* sceneManager) {
        this->sceneManager = sceneManager;
    }
};
