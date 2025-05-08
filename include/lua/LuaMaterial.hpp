#pragma once
#include "LuaSceneRef.hpp"
#include <string>
#include <SceneManager.hpp>
#include <filesystem>

class LuaMaterial : public LuaSceneRef {
  public:
    LuaMaterial(const std::string& diffuse, const std::string& normal)
        : LuaSceneRef(SceneManager::Get().AddMaterial(diffuse, normal)) {}
    ~LuaMaterial() = default;
};