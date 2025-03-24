/*
 * Copyright 2011-2019 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */
#include <stdio.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include "logo.h"
#include <Jolt/Jolt.h>
#include <glm/glm.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_events.h>

static bool s_showStats = false;

int main(int argc, char** argv) {
    // Initialize SDL with video support.
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create an SDL window.
    // Here we use SDL_WINDOW_VULKAN to indicate no default graphics context is created,
    // similar to GLFW_NO_API.
    SDL_Window* window = SDL_CreateWindow("helloworld",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          1024,
                                          768,
                                          SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
    bgfx::renderFrame();

    // Initialize bgfx using the native window handle and window resolution.
    bgfx::Init init;

    // Retrieve the native window handle from SDL.
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (!SDL_GetWindowWMInfo(window, &wmInfo)) {
        fprintf(stderr, "SDL_GetWindowWMInfo failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
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
    init.resolution.width  = (uint32_t)width;
    init.resolution.height = (uint32_t)height;
    init.resolution.reset  = BGFX_RESET_VSYNC;
    if (!bgfx::init(init)) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Set view 0 to the same dimensions as the window and to clear the color buffer.
    const bgfx::ViewId kClearView = 0;
    bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
    bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

    bool quit = false;
    SDL_Event event;
    while (!quit) {
        // Poll SDL events.
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EventType::SDL_QUIT)
                quit = true;
            else if (event.type == SDL_EventType::SDL_KEYDOWN) {
                // Toggle stats on F1 key release.
                if (event.key.keysym.sym == SDLK_F1)
                    s_showStats = !s_showStats;
            }
        }

        // Handle window resize.
        int newWidth, newHeight;
        SDL_GetWindowSize(window, &newWidth, &newHeight);
        if (newWidth != width || newHeight != height) {
            width = newWidth;
            height = newHeight;
            bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
            bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
        }

        // This dummy draw call is here to make sure that view 0 is cleared if
        // no other draw calls are submitted to view 0.
        bgfx::touch(kClearView);

        // Use debug font to print information about this example.
        bgfx::dbgTextClear();
        bgfx::dbgTextImage(bx::max<uint16_t>(uint16_t(width / 2 / 8), 20) - 20,
                           bx::max<uint16_t>(uint16_t(height / 2 / 16), 6) - 6,
                           40, 12, s_logo, 160);
        bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
        bgfx::dbgTextPrintf(0, 1, 0x0f,
                            "Color can be changed with ANSI "
                            "\x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;"
                            "mp\x1b[14;me\x1b[0m code too.");
        bgfx::dbgTextPrintf(
            80, 1, 0x0f,
            "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    "
            "\x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
        bgfx::dbgTextPrintf(
            80, 2, 0x0f,
            "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    "
            "\x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");
        const bgfx::Stats* stats = bgfx::getStats();
        bgfx::dbgTextPrintf(0, 2, 0x0f,
                            "Backbuffer %dW x %dH in pixels, debug text %dW x "
                            "%dH in characters.",
                            stats->width, stats->height, stats->textWidth,
                            stats->textHeight);

        // Enable stats or debug text.
        bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
        // Advance to next frame. Process submitted rendering primitives.
        bgfx::frame();
    }

    bgfx::shutdown();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
