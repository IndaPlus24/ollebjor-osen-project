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
        case SDL_EventType::SDL_KEYUP:
        case SDL_EventType::SDL_KEYDOWN:
            if (keyEventCallback == nullptr) {
                break;
            }
            keyEventCallback((Keycode)event.key.keysym.sym,
                             (KeyState)event.key.state);
            break;
        case SDL_EventType::SDL_MOUSEMOTION:
            if (mouseMoveEventCallback == nullptr) {
                break;
            }
            mouseMoveEventCallback(event.motion.x, event.motion.y,
                                   event.motion.xrel, event.motion.yrel);
            break;
        case SDL_EventType::SDL_MOUSEBUTTONDOWN:
        case SDL_EventType::SDL_MOUSEBUTTONUP:
            if (mouseButtonEventCallback == nullptr) {
                break;
            }
            mouseButtonEventCallback(event.button.x, event.button.y,
                                     (MouseButton)event.button.button,
                                     (KeyState)event.button.state);
            break;
        case SDL_EventType::SDL_MOUSEWHEEL:
            if (mouseWheelEventCallback == nullptr) {
                break;
            }
            mouseWheelEventCallback(event.wheel.x, event.wheel.y);
            break;
        case SDL_EventType::SDL_QUIT:
            quit = true;
            break;
        default:
            break;
        }
    }
}

void Core::SetKeyEventCallback(
    std::function<void(Keycode, KeyState)> callback) {
    keyEventCallback = callback;
}

void Core::SetMouseMoveEventCallback(
    std::function<void(int, int, int, int)> callback) {
    mouseMoveEventCallback = callback;
}

void Core::SetMouseButtonEventCallback(
    std::function<void(int, int, MouseButton, KeyState)> callback) {
    mouseButtonEventCallback = callback;
}

void Core::SetMouseWheelEventCallback(
    std::function<void(int, int)> callback) {
    mouseWheelEventCallback = callback;
}

double Core::GetDeltaTime() {
    now = bx::getHPCounter();
    double deltaTime = double(now - lastTime) / double(freq);
    lastTime = now;
    return deltaTime;
}
