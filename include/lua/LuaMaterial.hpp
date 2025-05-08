#pragma once
#include <string>
#include <SceneManager.hpp>
#include <filesystem>

class LuaMaterial {
  private:
    SceneRef<Material> m_sceneRef;

    bool IsValidPath(const std::string& path) const {
        return std::filesystem::exists(path);
    }

  public:
    LuaMaterial(const std::string& diffuse, const std::string& normal) {

        if (!IsValidPath(diffuse)) {
            throw std::invalid_argument("Invalid diffuse texture path: " + diffuse);
        }
        if (!IsValidPath(normal)) {
            throw std::invalid_argument("Invalid normal texture path: " + normal);
        }

        m_sceneRef = SceneManager::Get().AddMaterial(diffuse, normal);
    }
    uint64_t GetID() const {
        return m_sceneRef.id;
    }
    Material* GetMaterial() const {
        return m_sceneRef.data;
    }
};