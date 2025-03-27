#pragma once

#include <SDL2/SDL.h>
#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <cstdint>
#include <string>

class Renderer {
  private:
    SDL_Window* window;
    bgfx::ViewId clearView = 0;
    uint32_t width, height;
    std::string title;
    bgfx::VertexLayout layout;
    bgfx::ProgramHandle program;

  public:
    Renderer(std::string title, int width, int height);
    Renderer(Renderer&&) = default;
    Renderer(const Renderer&) = default;
    Renderer& operator=(Renderer&&) = default;
    Renderer& operator=(const Renderer&) = default;
    ~Renderer();

    bool Init();
    bool Shutdown();

    inline void GetWindowSize(uint32_t& w, uint32_t& h) {
        w = width;
        h = height;
    }
    inline bgfx::ProgramHandle& GetProgramHandle() { return program; }
    inline bgfx::VertexLayout& GetVertexLayout() { return layout; }

    void SetViewClear();
    bool UpdateWindowSize();
};
