#include "Renderer.hpp"
#include "ScreenVertex.hpp"
#include "SDL_syswm.h"
#include "SDL_video.h"
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bgfx/defines.h"
#include "bx/math.h"
#include <iostream>

#include <glsl/vs_geom.sc.bin.h>
#include <essl/vs_geom.sc.bin.h>
#include <spirv/vs_geom.sc.bin.h>
#include <glsl/fs_geom.sc.bin.h>
#include <essl/fs_geom.sc.bin.h>
#include <spirv/fs_geom.sc.bin.h>

#include <glsl/vs_light.sc.bin.h>
#include <essl/vs_light.sc.bin.h>
#include <spirv/vs_light.sc.bin.h>
#include <glsl/fs_light.sc.bin.h>
#include <essl/fs_light.sc.bin.h>
#include <spirv/fs_light.sc.bin.h>

#include <glsl/vs_combine.sc.bin.h>
#include <essl/vs_combine.sc.bin.h>
#include <spirv/vs_combine.sc.bin.h>
#include <glsl/fs_combine.sc.bin.h>
#include <essl/fs_combine.sc.bin.h>
#include <spirv/fs_combine.sc.bin.h>

#if BX_PLATFORM_WINDOWS
#include <dx11/vs_geom.sc.bin.h>
#include <dx11/fs_geom.sc.bin.h>
#include <dx11/vs_light.sc.bin.h>
#include <dx11/fs_light.sc.bin.h>
#include <dx11/vs_combine.sc.bin.h>
#include <dx11/fs_combine.sc.bin.h>

#endif // BX_PLATFORM_WINDOWS
#if BX_PLATFORM_OSX
#include <metal/vs_geom.sc.bin.h>
#include <metal/fs_geom.sc.bin.h>
#include <metal/vs_light.sc.bin.h>
#include <metal/fs_light.sc.bin.h>
#include <metal/vs_combine.sc.bin.h>
#include <metal/fs_combine.sc.bin.h>
#endif // BX_PLATFORM_OSX

static const ScreenVertex screenVertices[] = {
    {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  //
    {{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   //
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, //
    {{1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}};

static const uint16_t screenIndices[] = {0, 1, 2, //
                                         2, 1, 3};

Renderer::Renderer(std::string title, int width, int height)
    : width(width), height(height), title(title) {}

Renderer::~Renderer() {}

bool Renderer::Init() {
    window = SDL_CreateWindow(title.c_str(), width / 10, height / 10, width,
                              height, SDL_WINDOW_RESIZABLE);
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
    bgfx::setDebug(BGFX_DEBUG_TEXT | BGFX_DEBUG_PROFILER);

    bgfx::setPaletteColor(geometryView, 0x000000FF);
    bgfx::setPaletteColor(lightingView, 0x000000FF);
    bgfx::setPaletteColor(combineView, 0x000000FF);

    bgfx::setViewName(geometryView, "Geometry");
    bgfx::setViewClear(geometryView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x443355FF, 1.0f, 0);
    bgfx::setViewRect(geometryView, 0, 0, bgfx::BackbufferRatio::Equal);

    bgfx::setViewName(lightingView, "Lighting");
    bgfx::setViewClear(lightingView, BGFX_CLEAR_COLOR, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(lightingView, 0, 0, bgfx::BackbufferRatio::Equal);

    bgfx::setViewName(combineView, "Combine");
    bgfx::setViewClear(combineView, BGFX_CLEAR_COLOR, 0x443355FF, 1.0f, 0);
    bgfx::setViewRect(combineView, 0, 0, bgfx::BackbufferRatio::Equal);

    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Tangent, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    screenLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    screenVbh = bgfx::createVertexBuffer(
        bgfx::makeRef(screenVertices, sizeof(screenVertices)), screenLayout);
    screenIbh = bgfx::createIndexBuffer(
        bgfx::makeRef(screenIndices, sizeof(screenIndices)));

    texColorUniform =
        bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);
    texNormalUniform =
        bgfx::createUniform("s_texNormal", bgfx::UniformType::Sampler);

    albedoUniform = bgfx::createUniform("u_albedo", bgfx::UniformType::Sampler);
    normalUniform = bgfx::createUniform("u_normal", bgfx::UniformType::Sampler);
    depthUniform = bgfx::createUniform("u_depth", bgfx::UniformType::Sampler);

    lightingUniform =
        bgfx::createUniform("u_lighting", bgfx::UniformType::Sampler);

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    geometryProgram = bgfx::createProgram(
        bgfx::createShader(bgfx::makeRef(vs_geom_spv, sizeof(vs_geom_spv))),
        bgfx::createShader(bgfx::makeRef(fs_geom_spv, sizeof(vs_geom_spv))),
        true);
    lightingProgram = bgfx::createProgram(
        bgfx::createShader(bgfx::makeRef(vs_light_spv, sizeof(vs_light_spv))),
        bgfx::createShader(bgfx::makeRef(fs_light_spv, sizeof(fs_light_spv))),
        true);
    combineProgram = bgfx::createProgram(
        bgfx::createShader(
            bgfx::makeRef(vs_combine_spv, sizeof(vs_combine_spv))),
        bgfx::createShader(
            bgfx::makeRef(fs_combine_spv, sizeof(fs_combine_spv))),
        true);
#elif BX_PLATFORM_WINDOWS
    if (bgfx::getRendererType() == bgfx::RendererType::Direct3D11) {
        geometryProgram = bgfx::createProgram(
            bgfx::createShader(
                bgfx::makeRef(vs_geom_dx11, sizeof(vs_geom_dx11))),
            bgfx::createShader(
                bgfx::makeRef(fs_geom_dx11, sizeof(fs_geom_dx11))),
            true);
        lightingProgram = bgfx::createProgram(
            bgfx::createShader(
                bgfx::makeRef(vs_light_dx11, sizeof(vs_light_dx11))),
            bgfx::createShader(
                bgfx::makeRef(fs_light_dx11, sizeof(fs_light_dx11))),
            true);
        combineProgram = bgfx::createProgram(
            bgfx::createShader(
                bgfx::makeRef(vs_combine_dx11, sizeof(vs_combine_dx11))),
            bgfx::createShader(
                bgfx::makeRef(fs_combine_dx11, sizeof(fs_combine_dx11))),
            true);
    } else if (bgfx::getRendererType() == bgfx::RendererType::Vulkan) {
        geometryProgram = bgfx::createProgram(
            bgfx::createShader(bgfx::makeRef(vs_geom_spv, sizeof(vs_geom_spv))),
            bgfx::createShader(bgfx::makeRef(fs_geom_spv, sizeof(vs_geom_spv))),
            true);
        lightingProgram = bgfx::createProgram(
            bgfx::createShader(
                bgfx::makeRef(vs_light_spv, sizeof(vs_light_spv))),
            bgfx::createShader(
                bgfx::makeRef(fs_light_spv, sizeof(fs_light_spv))),
            true);
        combineProgram = bgfx::createProgram(
            bgfx::createShader(
                bgfx::makeRef(vs_combine_spv, sizeof(vs_combine_spv))),
            bgfx::createShader(
                bgfx::makeRef(fs_combine_spv, sizeof(fs_combine_spv))),
            true);
    } else {
        geometryProgram = bgfx::createProgram(
            bgfx::createShader(
                bgfx::makeRef(vs_geom_glsl, sizeof(vs_geom_glsl))),
            bgfx::createShader(
                bgfx::makeRef(fs_geom_glsl, sizeof(fs_geom_glsl))),
            true);
        lightingProgram = bgfx::createProgram(
            bgfx::createShader(
                bgfx::makeRef(vs_light_glsl, sizeof(vs_light_glsl))),
            bgfx::createShader(
                bgfx::makeRef(fs_light_glsl, sizeof(fs_light_glsl))),
            true);
        combineProgram = bgfx::createProgram(
            bgfx::createShader(
                bgfx::makeRef(vs_combine_glsl, sizeof(vs_combine_glsl))),
            bgfx::createShader(
                bgfx::makeRef(fs_combine_glsl, sizeof(fs_combine_glsl))),
            true);
    }
#elif BX_PLATFORM_OSX
    geometryProgram = bgfx::createProgram(
        bgfx::createShader(bgfx::makeRef(vs_geom_mtl, sizeof(vs_geom_mtl))),
        bgfx::createShader(bgfx::makeRef(fs_geom_mtl, sizeof(vs_geom_mtl))),
        true);
    lightingProgram = bgfx::createProgram(
        bgfx::createShader(bgfx::makeRef(vs_light_mtl, sizeof(vs_light_mtl))),
        bgfx::createShader(bgfx::makeRef(fs_light_mtl, sizeof(fs_light_mtl))),
        true);
    combineProgram = bgfx::createProgram(
        bgfx::createShader(
            bgfx::makeRef(vs_combine_mtl, sizeof(vs_combine_mtl))),
        bgfx::createShader(
            bgfx::makeRef(fs_combine_mtl, sizeof(fs_combine_mtl))),
        true);
#endif

    // Error-check program creation
    if (geometryProgram.idx == bgfx::kInvalidHandle ||
        lightingProgram.idx == bgfx::kInvalidHandle ||
        combineProgram.idx == bgfx::kInvalidHandle) {
        std::cerr << "Failed to create program" << std::endl;
        bgfx::shutdown();
        SDL_DestroyWindow(window);
        return false;
    }

    // Create frame buffer for G-buffers
    texGbuffers[0] = bgfx::createTexture2D(
        (uint32_t)width, (uint32_t)height, false, 1, bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP |
            BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    texGbuffers[1] = bgfx::createTexture2D(
        (uint32_t)width, (uint32_t)height, false, 1,
        bgfx::TextureFormat::RGBA16F,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP |
            BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);
    texGbuffers[2] = bgfx::createTexture2D(
        (uint32_t)width, (uint32_t)height, false, 1, bgfx::TextureFormat::D24F,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP |
            BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);

    // Create frame buffer for G-buffers
    GBuffersFrameBuffer = bgfx::createFrameBuffer(3, texGbuffers, true);

    // Create frame buffer for combine
    lightingFrameBuffer = bgfx::createFrameBuffer(
        (uint32_t)width, (uint32_t)height, bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP |
            BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT);

    // Set the view transform for the lighting/combine pass
    bx::mtxIdentity(identity);

    return true;
}
bool Renderer::Shutdown() {
    bgfx::destroy(screenVbh);
    bgfx::destroy(screenIbh);
    bgfx::destroy(geometryProgram);
    bgfx::destroy(lightingProgram);
    bgfx::destroy(combineProgram);
    bgfx::destroy(texColorUniform);
    bgfx::destroy(texNormalUniform);
    bgfx::destroy(albedoUniform);
    bgfx::destroy(normalUniform);
    bgfx::destroy(depthUniform);
    bgfx::destroy(lightingUniform);
    bgfx::destroy(GBuffersFrameBuffer);
    bgfx::destroy(lightingFrameBuffer);

    bgfx::shutdown();
    SDL_DestroyWindow(window);
    return true;
}

void Renderer::RecreateFrameBuffers(int width, int height) {
    this->width = width;
    this->height = height;
    bgfx::reset(width, height, BGFX_RESET_VSYNC);
    bgfx::setViewRect(geometryView, 0, 0, width, height);
    bgfx::setViewRect(lightingView, 0, 0, width, height);
    bgfx::setViewRect(combineView, 0, 0, width, height);

    bgfx::destroy(GBuffersFrameBuffer);
    bgfx::destroy(lightingFrameBuffer);

    texGbuffers[0] = bgfx::createTexture2D(
        (uint32_t)width, (uint32_t)height, false, 1, bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    texGbuffers[1] = bgfx::createTexture2D(
        (uint32_t)width, (uint32_t)height, false, 1,
        bgfx::TextureFormat::RGBA16F,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
    texGbuffers[2] = bgfx::createTexture2D(
        (uint32_t)width, (uint32_t)height, false, 1, bgfx::TextureFormat::D24F,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

    // Create frame buffer for G-buffers
    GBuffersFrameBuffer = bgfx::createFrameBuffer(3, texGbuffers, true);

    // Create frame buffer for combine
    lightingFrameBuffer = bgfx::createFrameBuffer(
        (uint32_t)width, (uint32_t)height, bgfx::TextureFormat::RGBA8,
        BGFX_TEXTURE_RT | BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);
}

void Renderer::BeginPass(bgfx::ViewId view) {
    currentView = view;
    switch (view) {
    case 0:
        BeginGeometry();
        break;
    case 1:
        BeginLighting();
        break;
    case 2:
        BeginCombine();
        break;
    default:
        break;
    }
}

void Renderer::EndPass() {
    switch (currentView) {
    case 0:
        bgfx::submit(geometryView, geometryProgram);
        break;
    case 1:
        bgfx::submit(lightingView, lightingProgram);
        break;
    case 2:
        bgfx::submit(combineView, combineProgram);
        break;
    default:
        break;
    }
}

void Renderer::BeginGeometry() {
    bgfx::setViewFrameBuffer(geometryView, GBuffersFrameBuffer);
    bgfx::setViewClear(geometryView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x000000ff, 1.0f, 0);
    bgfx::setViewRect(geometryView, 0, 0, width, height);

    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
                   BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS |
                   BGFX_STATE_MSAA);
}

void Renderer::BeginLighting() {
    bgfx::setViewFrameBuffer(lightingView, lightingFrameBuffer);
    bgfx::setViewClear(lightingView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x0000ffff, 1.0f, 0);
    bgfx::setViewRect(lightingView, 0, 0, bgfx::BackbufferRatio::Equal);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);

    bgfx::setViewTransform(lightingView, identity, identity);

    bgfx::setVertexBuffer(0, screenVbh);
    bgfx::setIndexBuffer(screenIbh);
    bgfx::setTexture(0, normalUniform,
                     bgfx::getTexture(GBuffersFrameBuffer, 1));
    bgfx::setTexture(1, depthUniform, bgfx::getTexture(GBuffersFrameBuffer, 2));
}

void Renderer::BeginCombine() {
    bgfx::setViewFrameBuffer(combineView, BGFX_INVALID_HANDLE);
    bgfx::setViewClear(combineView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
                       0x303030ff, 1.0f, 0);
    bgfx::setViewRect(combineView, 0, 0, width, height);
    bgfx::setViewTransform(combineView, identity, identity);
    bgfx::setTexture(0, albedoUniform,
                     bgfx::getTexture(GBuffersFrameBuffer, 0));
    bgfx::setTexture(1, lightingUniform,
                     bgfx::getTexture(lightingFrameBuffer, 0));

    bgfx::setVertexBuffer(0, screenVbh);
    bgfx::setIndexBuffer(screenIbh);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);
}

void Renderer::SetTextureUniforms(bgfx::TextureHandle albedo,
                                  bgfx::TextureHandle normal) {
    bgfx::setTexture(0, texColorUniform, albedo);
    bgfx::setTexture(1, texNormalUniform, normal);
}

void Renderer::SetTitle(std::string title) {
    this->title = title;
    SDL_SetWindowTitle(window, title.c_str());
}
