#pragma once
#include <glm/glm.hpp>
#include "Enums.hpp"
#include "LuaMaterial.hpp"
#include "LuaSceneRef.hpp"
#include "SceneManager.hpp"

class LuaPrimitive : public LuaSceneRef {
  public:
    LuaPrimitive(PrimitiveType type = PrimitiveType::Cube,
                 RigidBodyType bodyType = RigidBodyType::Dynamic);
    LuaPrimitive(PrimitiveType type, LuaMaterial& material,
                 RigidBodyType bodyType = RigidBodyType::Dynamic);
    ~LuaPrimitive();

    PrimitiveType GetType();
    void SetType(PrimitiveType type);

    void SetPosition(glm::vec3& position);
    glm::vec3 GetPosition();

    void SetRigidBodyType(RigidBodyType type);
    RigidBodyType GetRigidBodyType();
};