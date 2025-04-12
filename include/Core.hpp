#pragma once 

#include "Enums.hpp"
#include "bx/timer.h"
#include <SDL_events.h>
#include <functional>

class Core {
private:
    SDL_Event event;
    bool quit = false;
    const uint8_t* keyboardState = nullptr;
    uint8_t* lastKeyboardState = nullptr;
    bool keyboardActive = false;

    int64_t lastTime = bx::getHPCounter();
    int64_t now = bx::getHPCounter();
    int64_t freq = bx::getHPFrequency();

    std::function<void(Keycode, KeyState)> keyEventCallback;
    std::function<void(int, int, int, int)> mouseMoveEventCallback;
    std::function<void(int, int, MouseButton, KeyState)> mouseButtonEventCallback;
    std::function<void(int, int)> mouseWheelEventCallback;
    std::function<void(double)> physicsStepCallback;
    std::function<void(double)> updateCallback;
    
public:
    Core();
    Core(Core &&) = default;
    Core(const Core &) = default;
    Core &operator=(Core &&) = default;
    Core &operator=(const Core &) = default;
    ~Core();

    inline bool IsQuit() { return quit; }
    inline void SetQuit(bool q) { quit = q; }

    bool Init();
    bool Shutdown();

    void SetKeyEventCallback(std::function<void(Keycode, KeyState)> callback);
    void SetMouseMoveEventCallback(std::function<void(int, int, int, int)> callback);
    void SetMouseButtonEventCallback(std::function<void(int, int, MouseButton, KeyState)> callback);
    void SetMouseWheelEventCallback(std::function<void(int, int)> callback);
    void SetPhysicsStepCallback(std::function<void(double)> callback);
    void SetUpdateCallback(std::function<void(double)> callback);

    void EventLoop();
    double GetDeltaTime();
    void CallKeyboardEvent();
    void CallPhysicsStep(double deltaTime);
    void CallUpdate(double deltaTime);
    
};
