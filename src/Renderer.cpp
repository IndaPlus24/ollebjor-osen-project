#include "Renderer.hpp"
#include "SDL_syswm.h"
#include "SDL_video.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bgfx/defines.h"
#include <iostream>

#include <glsl/vs_cubes.sc.bin.h>
#include <essl/vs_cubes.sc.bin.h>
#include <spirv/vs_cubes.sc.bin.h>
#include <glsl/fs_cubes.sc.bin.h>
#include <essl/fs_cubes.sc.bin.h>
#include <spirv/fs_cubes.sc.bin.h>
#if BX_PLATFORM_WINDOWS
#include <dx11/vs_cubes.sc.bin.h>
#include <dx11/fs_cubes.sc.bin.h>
#endif // BX_PLATFORM_WINDOWS
#if BX_PLATFORM_OSX
#include <metal/vs_cubes.sc.bin.h>
#include <metal/fs_cubes.sc.bin.h>
#endif // BX_PLATFORM_OSX

Renderer::Renderer(std::string title, int width, int height)
    : width(width), height(height), title(title) {}

Renderer::~Renderer() {}

bool Renderer::Init() {
    window = SDL_CreateWindow(title.c_str(), 0, 0, width, height,
                              SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create
    // a render thread.
    bgfx::renderFrame();

    bgfx::Init init;
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (!SDL_GetWindowWMInfo(window, &wmInfo)) {
        std::cerr << "SDL_GetWindowWMInfo failed: " << SDL_GetError()
                  << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    if (wmInfo.subsystem == SDL_SYSWM_WAYLAND) {
        init.platformData.type = bgfx::NativeWindowHandleType::Wayland;
        init.platformData.ndt = wmInfo.info.wl.display;
        init.platformData.nwh = wmInfo.info.wl.surface;
    } else {
        init.platformData.type = bgfx::NativeWindowHandleType::Default;
        init.platformData.ndt = wmInfo.info.x11.display;
        init.platformData.nwh = (void*)(uintptr_t)wmInfo.info.x11.window;
    }
    init.type = bgfx::RendererType::Vulkan;
#elif BX_PLATFORM_WINDOWS
    init.platformData.nwh = wmInfo.info.win.window;
#elif BX_PLATFORM_OSX
    init.platformData.nwh = wmInfo.info.cocoa.window;
    init.type = bgfx::RendererType::Metal;
#endif

    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    init.resolution.width = (uint32_t)width;
    init.resolution.height = (uint32_t)height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    if (!bgfx::init(init)) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    uint64_t state = 0 | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_RGB |
                     BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
                     BGFX_STATE_FRONT_CCW;
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    textureUniform =
        bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    program = bgfx::createProgram(
        bgfx::createShader(bgfx::copy(vs_cubes_spv, sizeof(vs_cubes_spv))),
        bgfx::createShader(bgfx::copy(fs_cubes_spv, sizeof(fs_cubes_spv))),
        true);
#elif BX_PLATFORM_WINDOWS
    if (bgfx::getRendererType() == bgfx::RendererType::Direct3D11) {
        program = bgfx::createProgram(
            bgfx::createShader(
                bgfx::copy(vs_cubes_dx11, sizeof(vs_cubes_dx11))),
            bgfx::createShader(
                bgfx::copy(fs_cubes_dx11, sizeof(fs_cubes_dx11))),
            true);
    } else if (bgfx::getRendererType() == bgfx::RendererType::Vulkan) {
        program = bgfx::createProgram(
            bgfx::createShader(bgfx::copy(vs_cubes_spv, sizeof(vs_cubes_spv))),
            bgfx::createShader(bgfx::copy(fs_cubes_spv, sizeof(fs_cubes_spv))),
            true);
    } else {
        program = bgfx::createProgram(
            bgfx::createShader(
                bgfx::copy(vs_cubes_glsl, sizeof(vs_cubes_glsl))),
            bgfx::createShader(
                bgfx::copy(fs_cubes_glsl, sizeof(fs_cubes_glsl))),
            true);
    }
#elif BX_PLATFORM_OSX
    program = bgfx::createProgram(
        bgfx::createShader(bgfx::copy(vs_cubes_mtl, sizeof(vs_cubes_mtl))),
        bgfx::createShader(bgfx::copy(fs_cubes_mtl, sizeof(vs_cubes_mtl))),
        true);
#endif
    if (program.idx == bgfx::kInvalidHandle) {
        std::cerr << "Failed to create shaders" << std::endl;
        bgfx::shutdown();
        SDL_DestroyWindow(window);
        return false;
    }

    return true;
}
bool Renderer::Shutdown() {

    bgfx::destroy(program);
    bgfx::destroy(textureUniform);

    bgfx::shutdown();
    SDL_DestroyWindow(window);
    return true;
}

void Renderer::SetTextureUniform(bgfx::TextureHandle texture) {
    if (texture.idx == bgfx::kInvalidHandle) {
        std::cerr << "Invalid texture handle" << std::endl;
        return;
    }
    bgfx::setTexture(0, textureUniform, texture, 0);
}

void Renderer::SetTitle(std::string title) {
    this->title = title;
    SDL_SetWindowTitle(window, title.c_str());
}

void Renderer::SetViewClear() {
    bgfx::setViewClear(clearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x443355FF, 1.0f, 0);
    bgfx::setViewRect(clearView, 0, 0, bgfx::BackbufferRatio::Equal);
}

bool Renderer::UpdateWindowSize() {
    int newWidth, newHeight;
    SDL_GetWindowSize(window, &newWidth, &newHeight);
    if (newWidth != (int)width || newHeight != (int)height) {
        width = newWidth;
        height = newHeight;
        bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
        bgfx::setViewRect(clearView, 0, 0, bgfx::BackbufferRatio::Equal);
    }
    return true;
}
