#include "Core.hpp"

#include "EventDispatcher.hpp"
#include <SDL2/SDL.h>
#include "Enums.hpp"
#include "SDL_keyboard.h"
#include "SDL_video.h"
#include "bx/debug.h"
#include <cstddef>
#include <iostream>

Core::Core() {}

Core::~Core() {}

bool Core::Init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    lastKeyboardState = new uint8_t[SDL_NUM_SCANCODES];
    return true;
}
bool Core::Shutdown() {
    if (lastKeyboardState != nullptr) {
        delete[] lastKeyboardState;
        lastKeyboardState = nullptr;
    }
    SDL_Quit();
    return true;
}
void Core::EventLoop() {
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EventType::SDL_KEYUP:
            memcpy(lastKeyboardState, keyboardState,
                   SDL_NUM_SCANCODES * sizeof(uint8_t));
            keyboardState = SDL_GetKeyboardState(nullptr);
            keyboardActive = false;
            if (keyEventCallback == nullptr)
                break;
            keyEventCallback((Keycode)event.key.keysym.scancode,
                             KeyState::Release);
            break;
        case SDL_EventType::SDL_KEYDOWN:
            if (event.key.repeat == 1)
                break;
            keyboardState = SDL_GetKeyboardState(nullptr);
            keyboardActive = true;
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
        case SDL_EventType::SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                renderer->RecreateFrameBuffers(event.window.data1,
                                               event.window.data2);
            } else if (event.window.event == SDL_WINDOWEVENT_MINIMIZED) {
                bx::debugPrintf("Window minimized\n");
                WindowMinimized();
                bx::debugPrintf("Minimized event sent\n");
            }
            break;
        default:
            break;
        }
    }
}

struct KeyEvent {
    Keycode keycode;
    KeyState state;
};

void Core::CallKeyboardEvent() {
    if (keyboardState == nullptr || keyEventCallback == nullptr ||
        !keyboardActive) {
        return;
    }
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (keyboardState[i] == 1 && lastKeyboardState[i] == 0) {
            keyEventCallback((Keycode)i, KeyState::Pressed);
        }
    }
}

void Core::CallPhysicsStep(double deltaTime) {
    if (physicsStepCallback == nullptr) {
        return;
    }
    physicsStepCallback(deltaTime);
}

void Core::CallUpdate(double deltaTime) {
    if (updateCallback == nullptr) {
        return;
    }
    updateCallback(deltaTime);
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

void Core::SetMouseWheelEventCallback(std::function<void(int, int)> callback) {
    mouseWheelEventCallback = callback;
}

void Core::SetPhysicsStepCallback(std::function<void(double)> callback) {
    physicsStepCallback = callback;
}

void Core::SetUpdateCallback(std::function<void(double)> callback) {
    updateCallback = callback;
}

double Core::GetDeltaTime() {
    now = bx::getHPCounter();
    double deltaTime = double(now - lastTime) / double(freq);
    lastTime = now;
    return deltaTime;
}
