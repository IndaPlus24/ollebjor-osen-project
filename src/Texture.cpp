#include "Texture.hpp"
#include <cstdlib>
#include <format>
#include <iostream>
#include <string>
#include "bgfx/bgfx.h"
#include "bx/debug.h"
#include "stb_image.h"
#define USE_FMT true
#include "GameEngineLogger.hpp"

Texture::Texture()
    : filePath(""), textureHandle(bgfx::kInvalidHandle), mem(nullptr), width(0),
      height(0), numMips(0), numLayers(0), flags(0),
      format(bgfx::TextureFormat::Unknown) {}

Texture::Texture(const std::string& filePath, bgfx::TextureFormat::Enum format,
                 uint32_t flags)
    : filePath(filePath), format(format), flags(flags) {
    int width, height, BPP;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &BPP, 0);
    GameLogger::LogManager::Instance().GetOrCreateChannel(
        "Texture", GameLogger::LogLevel::DEBUG);
    if (!data) {
        LOG_ERROR("Texture",
                  std::format("Failed to load texture: {}", filePath.c_str()));
        return;
    } else {
        LOG_DEBUG("Texture", std::format("Loaded texture: {}.", filePath));
    }
    mem = bgfx::copy(data, width * height * BPP);
    stbi_image_free(data);
    LOG_DEBUG("Texture", "copied and freed");
    textureHandle = bgfx::createTexture2D(
        width, height, false, 1, format,
        flags | BGFX_TEXTURE_RT | BGFX_SAMPLER_UVW_CLAMP |
            BGFX_SAMPLER_MAG_ANISOTROPIC | BGFX_SAMPLER_MIN_ANISOTROPIC);
    LOG_DEBUG("Texture", "Handle created");
    if (bgfx::isValid(textureHandle)) {
        bgfx::updateTexture2D(textureHandle, 0, 0, 0, 0, width, height, mem,
                              width * 4);
    } else {
        LOG_ERROR("Texture", std::format("Failed to create texture: {}",
                                         filePath.c_str()));
        exit(1);
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

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
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
    return *this;
}

Texture::~Texture() {
    if (bgfx::isValid(textureHandle)) {
        bgfx::destroy(textureHandle);
        LOG_DEBUG("Texture",
                  std::format("Texture destroyed: {}", filePath.c_str()));
    }
}
