#pragma once
#include "Collider.hpp"
#include "Enums.hpp"
#include "LuaSceneRef.hpp"
#include "SceneManager.hpp"
#include "glm/fwd.hpp"
#include "lua/LuaSceneRef.hpp"

class LuaCollider : public LuaSceneRef {
  public:
    LuaCollider(ColliderType type = ColliderType::Box, glm::vec3 position = glm::vec3(0.0),
                glm::vec3 rotation = glm::vec3(0.0),
                glm::vec3 scale = glm::vec3(1.0))
        : LuaSceneRef(SceneManager::Get().AddCollider(type, position, rotation,
                                                      scale)) {}
    ~LuaCollider() = default;
};