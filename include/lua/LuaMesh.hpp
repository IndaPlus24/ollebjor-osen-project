#pragma once

#include "Enums.hpp"
#include "LuaCollider.hpp"
#include "LuaMaterial.hpp"
#include "LuaMeshContainer.hpp"
#include "MeshEntity.hpp"
#include "SceneManager.hpp"
#include "lua/LuaSceneRef.hpp"

class LuaMesh : public LuaSceneRef {
  public:
    LuaMesh(LuaMeshContainer& meshContainer, LuaCollider& collider,
            LuaMaterial& material, glm::vec3 position = glm::vec3(0.0f),
            RigidBodyType bodyType = RigidBodyType::Dynamic);
    ~LuaMesh() = default;
};