#pragma once
#include <glm/glm.hpp>
#include "Entity.hpp"
#include "LuaMaterial.hpp"
#include "SceneManager.hpp"
#include "LuaSceneRef.hpp"

class LuaPrimitive : public LuaSceneRef<Entity> {
  public:
    LuaPrimitive(PrimitiveType type, glm::vec3 position = glm::vec3(0.0f));
    LuaPrimitive(PrimitiveType type, LuaMaterial& material,
                 glm::vec3 position = glm::vec3(0.0f));
    ~LuaPrimitive();

    PrimitiveType GetType();
    void SetType(PrimitiveType type);

    void SetPosition(glm::vec3& position);
    glm::vec3 GetPosition();
    void Destroy();
};