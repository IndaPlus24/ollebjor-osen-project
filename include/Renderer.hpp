#pragma once

#include <SDL2/SDL.h>
#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <cstdint>
#include <string>

class Renderer {
  private:
    SDL_Window* window;
    uint32_t width, height;
    std::string title;

    bgfx::ViewId geometryView = 0;
    bgfx::ViewId lightingView = 1;
    bgfx::ViewId combineView = 2;

    bgfx::VertexLayout layout;

    bgfx::VertexLayout screenLayout;
    bgfx::VertexBufferHandle screenVbh;
    bgfx::IndexBufferHandle screenIbh;

    bgfx::ProgramHandle geometryProgram;
    bgfx::ProgramHandle lightingProgram;
    bgfx::ProgramHandle combineProgram;

    bgfx::TextureHandle texGbuffers[3];
    bgfx::FrameBufferHandle GBuffersFrameBuffer;

    bgfx::FrameBufferHandle combineFrameBuffer;

    bgfx::UniformHandle texColorUniform;
    bgfx::UniformHandle texNormalUniform;

    bgfx::UniformHandle albedoUniform;
    bgfx::UniformHandle normalUniform;
    bgfx::UniformHandle depthUniform;
    bgfx::UniformHandle lightingUniform;

  public:
    Renderer(std::string title, int width, int height);
    Renderer(Renderer&&) = default;
    Renderer(const Renderer&) = default;
    Renderer& operator=(Renderer&&) = default;
    Renderer& operator=(const Renderer&) = default;
    ~Renderer();

    bool Init();
    bool Shutdown();
    void RecreateFrameBuffers();

    inline void GetWindowSize(uint32_t& w, uint32_t& h) {
        w = width;
        h = height;
    }

    inline bgfx::VertexLayout& GetVertexLayout() { return layout; }
    inline float GetAspectRatio() {
        return static_cast<float>(width) / static_cast<float>(height);
    }

    void SetTextureUniforms(bgfx::TextureHandle albedo,
                            bgfx::TextureHandle normal);

    void SetViewClear();
    bool UpdateWindowSize();
    void SetTitle(std::string title);
};
