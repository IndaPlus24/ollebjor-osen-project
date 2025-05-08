#pragma once
#include "SceneManager.hpp"
#include <cstdint>

class LuaSceneRef {
  public:
    template <typename T> LuaSceneRef(SceneRef<T> sceneRef) {
        m_id = sceneRef.id;
        m_data = sceneRef.data;
    };
    ~LuaSceneRef() = default;
    uint64_t GetId() const { return m_id; }
    template <typename T> T* Get() { return static_cast<T*>(m_data); }
    template <typename T> void Destroy() {
        SceneManager::Get().RemoveSceneRef<T>(m_id);
        m_data = nullptr;
        m_id = -1;
    }

  private:
    uint64_t m_id;
    void* m_data;
};