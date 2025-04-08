#include "Texture.hpp"
#include <iostream>
#include "bgfx/bgfx.h"
#include "bx/debug.h"
#include "stb_image.h"

Texture::Texture()
    : filePath(""), textureHandle(bgfx::kInvalidHandle), mem(nullptr), width(0),
      height(0), numMips(0), numLayers(0), flags(0),
      format(bgfx::TextureFormat::Unknown) {}

Texture::Texture(const std::string& filePath, bgfx::TextureFormat::Enum format,
                 uint32_t flags)
    : filePath(filePath), format(format), flags(flags) {
    int width, height, BPP;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &BPP, 0);

    if (!data) {
        bx::debugPrintf("Failed to load texture: %s", filePath.c_str());
        return;
    } else {
        bx::debugPrintf("Loaded texture: %s (%dx%d), BPP: %d", filePath.c_str(), width,
                        height, BPP);
    }
    mem = bgfx::copy(data, width * height * BPP);
    stbi_image_free(data);

    textureHandle =
        bgfx::createTexture2D(width, height, false, 1, format,
                              flags | BGFX_TEXTURE_RT | BGFX_SAMPLER_UVW_CLAMP | BGFX_SAMPLER_MAG_ANISOTROPIC |
                                  BGFX_SAMPLER_MIN_ANISOTROPIC);
    if (bgfx::isValid(textureHandle)) {
        bgfx::updateTexture2D(textureHandle, 0, 0, 0, 0, width, height, mem,
                              width * 4);
    } else {
        std::cerr << "Failed to create texture: " << filePath << std::endl;
    }
}

Texture::Texture(Texture&& other) noexcept {
    filePath = std::move(other.filePath);
    textureHandle = other.textureHandle;
    mem = other.mem;
    width = other.width;
    height = other.height;
    numMips = other.numMips;
    numLayers = other.numLayers;
    flags = other.flags;
    format = other.format;

    other.textureHandle.idx = bgfx::kInvalidHandle;
    other.mem = nullptr;
}
Texture::~Texture() {
    if (bgfx::isValid(textureHandle)) {
        bgfx::destroy(textureHandle);
        bx::debugPrintf("Texture destroyed: %s", filePath.c_str());
    }
}
