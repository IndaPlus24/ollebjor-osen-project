#include "Renderer.hpp"
#include "SDL_syswm.h"
#include "SDL_video.h"
#include "Vertex.hpp"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bgfx/defines.h"
#include <iostream>

#include <bgfx/embedded_shader.h>
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
    init.platformData.ndt = wmInfo.info.x11.display;
    init.platformData.nwh = (void*)(uintptr_t)wmInfo.info.x11.window;
#elif BX_PLATFORM_WINDOWS
    init.platformData.nwh = wmInfo.info.win.window;
#elif BX_PLATFORM_OSX
    init.platformData.nwh = wmInfo.info.cocoa.window;
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
    uint64_t state = 0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                     BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
                     BGFX_STATE_MSAA | BGFX_STATE_PT_TRISTRIP;
    bgfx::setState(state);
    // bgfx::setDebug(BGFX_DEBUG_STATS);

    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .end();

    const bgfx::EmbeddedShader vertexShader = BGFX_EMBEDDED_SHADER(vs_cubes);
    const bgfx::EmbeddedShader fragmentShader = BGFX_EMBEDDED_SHADER(fs_cubes);

    program = bgfx::createProgram(
        bgfx::createEmbeddedShader(&vertexShader, bgfx::getRendererType(),
                                   "vs_cubes"),
        bgfx::createEmbeddedShader(&fragmentShader, bgfx::getRendererType(),
                                   "fs_cubes"),
        true);

    return true;
}
bool Renderer::Shutdown() {

    bgfx::destroy(program);

    bgfx::shutdown();
    SDL_DestroyWindow(window);
    return true;
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
