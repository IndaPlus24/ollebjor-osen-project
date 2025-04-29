#pragma once

#include <cstdint>
#include <glm/glm.hpp>

class Material {
  private:
    uint64_t albedoId;
    uint64_t normalId;

  public:
    Material(uint64_t albedoId, uint64_t normalId)
        : albedoId(albedoId), normalId(normalId) {};
    Material(Material&&) = default;
    Material(const Material&) = default;
    Material& operator=(Material&&) = default;
    Material& operator=(const Material&) = default;
    ~Material() = default;

    inline uint64_t GetAlbedoId() const { return albedoId; }
    inline uint64_t GetNormalId() const { return normalId; }
};
