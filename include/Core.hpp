#pragma once 

#include "bx/timer.h"
#include <SDL_events.h>

class Core {
private:
    SDL_Event event;
    bool quit = false;

    int64_t lastTime = bx::getHPCounter();
    int64_t now = bx::getHPCounter();
    int64_t freq = bx::getHPFrequency();
    
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

    void EventLoop();
    void KeyDownEvent(SDL_Keycode key);
    double GetDeltaTime();
    
};
