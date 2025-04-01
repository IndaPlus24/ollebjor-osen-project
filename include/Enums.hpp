#pragma once

#include "SDL_scancode.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

enum class PrimitiveType {
    Cube,
    Plane,
    Sphere
};

enum class RigidBodyType {
    Static,
    Dynamic,
    Kinematic
};

enum class Keycode {
    UNKNOWN = SDLK_UNKNOWN,
    RETURN = SDL_SCANCODE_RETURN,
    ESCAPE = SDL_SCANCODE_ESCAPE,
    BACKSPACE = SDL_SCANCODE_BACKSPACE,
    TAB = SDL_SCANCODE_TAB,
    CAPSLOCK = SDL_SCANCODE_CAPSLOCK,
    SPACE = SDL_SCANCODE_SPACE,
    PERCENT = SDL_SCANCODE_KP_PERCENT,
    AMPERSAND = SDL_SCANCODE_KP_AMPERSAND,
    LEFTPAREN = SDL_SCANCODE_KP_LEFTPAREN,
    RIGHTPAREN = SDL_SCANCODE_KP_RIGHTPAREN,
    COMMA = SDL_SCANCODE_COMMA,
    MINUS = SDL_SCANCODE_MINUS,
    PERIOD = SDL_SCANCODE_PERIOD,
    SLASH = SDL_SCANCODE_SLASH,
    NUM_0 = SDL_SCANCODE_0,
    NUM_1 = SDL_SCANCODE_1,
    NUM_2 = SDL_SCANCODE_2,
    NUM_3 = SDL_SCANCODE_3,
    NUM_4 = SDL_SCANCODE_4,
    NUM_5 = SDL_SCANCODE_5,
    NUM_6 = SDL_SCANCODE_6,
    NUM_7 = SDL_SCANCODE_7,
    NUM_8 = SDL_SCANCODE_8,
    NUM_9 = SDL_SCANCODE_9,
    SEMICOLON = SDL_SCANCODE_SEMICOLON,
    EQUALS = SDL_SCANCODE_EQUALS,
    LEFTBRACKET = SDL_SCANCODE_LEFTBRACKET,
    BACKSLASH = SDL_SCANCODE_BACKSLASH,
    RIGHTBRACKET = SDL_SCANCODE_RIGHTBRACKET,
    A = SDL_SCANCODE_A,
    B = SDL_SCANCODE_B,
    C = SDL_SCANCODE_C,
    D = SDL_SCANCODE_D,
    E = SDL_SCANCODE_E,
    F = SDL_SCANCODE_F,
    G = SDL_SCANCODE_G,
    H = SDL_SCANCODE_H,
    I = SDL_SCANCODE_I,
    J = SDL_SCANCODE_J,
    K = SDL_SCANCODE_K,
    L = SDL_SCANCODE_L,
    M = SDL_SCANCODE_M,
    N = SDL_SCANCODE_N,
    O = SDL_SCANCODE_O,
    P = SDL_SCANCODE_P,
    Q = SDL_SCANCODE_Q,
    R = SDL_SCANCODE_R,
    S = SDL_SCANCODE_S,
    T = SDL_SCANCODE_T,
    U = SDL_SCANCODE_U,
    V = SDL_SCANCODE_V,
    W = SDL_SCANCODE_W,
};

enum class KeyState {
    Pressed = SDL_PRESSED,
    Release = SDL_RELEASED
};

enum class MouseButton {
    Left = SDL_BUTTON_LEFT,
    Middle = SDL_BUTTON_MIDDLE,
    Right = SDL_BUTTON_RIGHT,
    X1 = SDL_BUTTON_X1,
    X2 = SDL_BUTTON_X2
};
