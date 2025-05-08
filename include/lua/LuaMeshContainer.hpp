#pragma once
#include "SceneManager.hpp"
#include "lua/LuaSceneRef.hpp"

class LuaMeshContainer : public LuaSceneRef {
  public:
    LuaMeshContainer(std::string path)
        : LuaSceneRef(SceneManager::Get().AddMeshContainer(path)) {}
    ~LuaMeshContainer() = default;
};