#pragma once
#include "Entity.hpp"
#include "SceneManager.hpp"
#include <cstdint>

template <typename T> class LuaSceneRef {
  public:
    LuaSceneRef(SceneRef<T> sceneRef) : m_sceneRef(sceneRef) {}
    ~LuaSceneRef() {}

    uint64_t GetId() const { return m_sceneRef.id; }
    T* Get() const { return m_sceneRef.data; }
    void Destroy() {
        SceneManager::Get().RemoveEntity(GetId());
        m_sceneRef = SceneRef<T>();
    }

  private:
    SceneRef<T> m_sceneRef;
};