#include "SceneImporter.hpp"
#include "Entity.hpp"
#include "SceneManager.hpp"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "utils.hpp"
#include <cstdint>
#include <iostream>
#include <vector>

void SceneImporter::processNode(aiNode* node, const aiScene* scene) {
    for (uint32_t i = 0; i < node->mNumMeshes; i++) {
        uint64_t matId = 0;
        auto meshRef =
            processMesh(scene->mMeshes[node->mMeshes[i]], scene, matId);
        if (meshRef.data) {
            auto colliderRef = sceneManager->AddCollider(
                Collider(ColliderType::Mesh, glm::vec3(0.0f), glm::vec3(0.0f),
                         glm::vec3(1.0f), meshRef.data->GetVertices(),
                         meshRef.data->GetIndices()));
            auto ref = sceneManager->AddEntity(meshRef.id, colliderRef.id,
                                              RigidBodyType::Static, matId);
            ref.data->SetTransform(ToGLM(node->mTransformation));
            if (ref.data) {
                sceneRefs.push_back(std::move(ref));
            } else {
                std::cerr << "ERROR: Failed to add entity" << std::endl;
            }
        }
    }
    for (uint32_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

SceneRef<MeshContainer> SceneImporter::processMesh(aiMesh* mesh,
                                                   const aiScene* scene,
                                                   uint64_t& matId) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.pos = vector;

        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = vec;
        } else {
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->HasTangentsAndBitangents()) {
            glm::vec3 vec;
            vec.x = mesh->mTangents[i].x;
            vec.y = mesh->mTangents[i].y;
            vec.z = mesh->mTangents[i].z;
            vertex.tangent = vec;
        }
        vertices.push_back(vertex);
    }

    for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        uint64_t diffuse =
            loadMaterialTextures(material, aiTextureType_DIFFUSE);
        uint64_t normal = loadMaterialTextures(material, aiTextureType_NORMALS);

        auto mat = sceneManager->AddMaterial(diffuse, normal);
        matId = mat.id;
    }

    if (vertices.size() > 0 && indices.size() > 0) {
        auto meshRef = sceneManager->AddMeshContainer(
            MeshContainer(workingDirectory + "/" + mesh->mName.C_Str(),
                          std::move(vertices), std::move(indices)));
        return meshRef;
    }
    std::cerr << "ERROR: Mesh has no vertices or indices" << std::endl;
    return SceneRef<MeshContainer>();
}

uint64_t SceneImporter::loadMaterialTextures(aiMaterial* mat,
                                             aiTextureType type) {
    aiString str;
    mat->GetTexture(type, 0, &str);
    std::string path = str.C_Str();
    auto textureRef = sceneManager->AddTexture(workingDirectory + "/" + path);
    if (textureRef.data) {
        return textureRef.id;
    } else if (type == aiTextureType_NORMALS) {
        std::cerr << "ERROR: Normal texture not found: " << path << std::endl;
        return 1;
    } else {
        std::cerr << "ERROR: Texture not found: " << path << std::endl;
        return 0;
    }
}

std::vector<SceneRef<Entity>> SceneImporter::ImportScene(const std::string& path) {
    sceneRefs.clear();
    Assimp::Importer importer;
    const aiScene* scene =
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals |
                                    aiProcess_CalcTangentSpace);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
        !scene->mRootNode) {
        std::cerr << "ERROR Scene import failed: " << importer.GetErrorString()
                  << std::endl;
        return {};
    }
    workingDirectory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
    return sceneRefs;
}
