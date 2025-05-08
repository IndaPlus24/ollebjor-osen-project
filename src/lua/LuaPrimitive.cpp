#include "lua/LuaPrimitive.hpp"

#include "Entity.hpp"
#include "SceneManager.hpp"
#include <iostream>
#include <glm/glm.hpp>

LuaPrimitive::LuaPrimitive(PrimitiveType type, glm::vec3 position)
    : LuaSceneRef<Entity>(SceneManager::Get().AddEntity(
          type, RigidBodyType::Dynamic, 0, position)) {}

LuaPrimitive::LuaPrimitive(PrimitiveType type, LuaMaterial& material,
                           glm::vec3 position)
    : LuaSceneRef<Entity>(SceneManager::Get().AddEntity(
          type, RigidBodyType::Dynamic, material.GetID(), position)) {};

LuaPrimitive::~LuaPrimitive() {
    std::cout << "LuaPrimitive destructor called for primitive" << std::endl;
}

void LuaPrimitive::SetPosition(glm::vec3& position) {
    Get()->SetPhysicsPosition(position);
}
glm::vec3 LuaPrimitive::GetPosition() { return Get()->GetPosition(); }

PrimitiveType LuaPrimitive::GetType() {
    Primitive* p = dynamic_cast<Primitive*>(Get());
    return p->GetType();
}

void LuaPrimitive::SetType(PrimitiveType type) {
    SceneManager::Get().UpdateEntity(GetId(), type);
}