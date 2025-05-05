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

  public:
    Texture();
    Texture(const std::string& filePath, uint32_t flags = 0);
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    ~Texture();

    inline bgfx::TextureHandle GetTextureHandle() const {
        return textureHandle;
    }

    inline const std::string& GetPath() const { return filePath; }
};
