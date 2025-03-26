#include "Core.hpp"

#include <SDL2/SDL.h>
#include "Enums.hpp"
#include <iostream>

Core::Core() {}

Core::~Core() {}

bool Core::Init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    return true;
}
bool Core::Shutdown() {
    SDL_Quit();
    return true;
}
void Core::EventLoop() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EventType::SDL_KEYDOWN:
                KeyDownEvent(event.key.keysym.sym);
                break;
            case SDL_EventType::SDL_QUIT:
                quit = true;
                break;
            default:
                break;
        }
    }
}

void Core::KeyDownEvent(SDL_Keycode key) {
    switch (key) {
        case SDLK_ESCAPE:
            quit = true;
            break;
        case SDLK_F1:
            break;
        default:
            break;

    }
}
