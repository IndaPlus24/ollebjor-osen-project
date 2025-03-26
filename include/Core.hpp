#pragma once 

#include <SDL_events.h>

class Core {
private:
    SDL_Event event;
    bool quit = false;
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
    
};
