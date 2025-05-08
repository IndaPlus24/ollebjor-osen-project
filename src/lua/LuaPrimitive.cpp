#include "lua/LuaPrimitive.hpp"

#include "Entity.hpp"
#include "Enums.hpp"
#include "Primitive.hpp"
#include "SceneManager.hpp"
#include <iostream>
#include <glm/glm.hpp>

LuaPrimitive::LuaPrimitive(PrimitiveType type, RigidBodyType bodyType)
    : LuaSceneRef(SceneManager::Get().AddEntity(type, bodyType, 0)) {}

LuaPrimitive::LuaPrimitive(PrimitiveType type, LuaMaterial& material,
                           RigidBodyType bodyType)
    : LuaSceneRef(
          SceneManager::Get().AddEntity(type, bodyType, material.GetId())) {};

LuaPrimitive::~LuaPrimitive() {
    std::cout << "LuaPrimitive destructor called for primitive" << std::endl;
}

void LuaPrimitive::SetPosition(glm::vec3& position) {
    Get<Entity>()->SetPhysicsPosition(position);
}
glm::vec3 LuaPrimitive::GetPosition() { return Get<Entity>()->GetPosition(); }

PrimitiveType LuaPrimitive::GetType() {
    Primitive* p = dynamic_cast<Primitive*>(Get<Entity>());
    return p->GetType();
}

void LuaPrimitive::SetType(PrimitiveType type) {
    SceneManager::Get().UpdateEntity(GetId(), type);
}

RigidBodyType LuaPrimitive::GetRigidBodyType() {
    Primitive* p = dynamic_cast<Primitive*>(Get<Entity>());
    return p->GetBodyType();
}
void LuaPrimitive::SetRigidBodyType(RigidBodyType type) {
    Primitive* p = dynamic_cast<Primitive*>(Get<Entity>());
    p->SetBodyType(type);
}