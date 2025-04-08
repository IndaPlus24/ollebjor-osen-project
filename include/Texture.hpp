#pragma once

#include <string>
#include <bgfx/bgfx.h>

class Texture {
  private:
    std::string filePath;
    bgfx::TextureHandle textureHandle;
    const bgfx::Memory* mem;
    uint32_t width;
    uint32_t height;
    uint32_t numMips;
    uint32_t numLayers;
    uint32_t flags;
    bgfx::TextureFormat::Enum format;

  public:
    Texture();
    Texture(const std::string& filePath,
            bgfx::TextureFormat::Enum format = bgfx::TextureFormat::Unknown, uint32_t flags = 0);
    Texture(Texture&& other) noexcept;
    Texture(const Texture&) = default;
    Texture& operator=(Texture&&) = default;
    Texture& operator=(const Texture&) = default;
    ~Texture();

    inline bgfx::TextureHandle GetTextureHandle() const {
        return textureHandle;
    }
};
