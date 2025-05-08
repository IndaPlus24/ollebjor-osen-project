#pragma once

#include "LuaSceneRef.hpp"
#include "SceneManager.hpp"

class LuaCamera : public LuaSceneRef {
  public:
    LuaCamera() : LuaSceneRef(SceneManager::Get().AddCamera()) {};
    ~LuaCamera() = default;

    void SetCurrent() { SceneManager::Get().SetActiveCamera(GetId()); }
};