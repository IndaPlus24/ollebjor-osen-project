#pragma once

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>

enum class PrimitiveType {
    Cube,
    Plane,
    Sphere
};

enum class Keycode {
    UNKNOWN = SDLK_UNKNOWN,
    RETURN = SDLK_RETURN,
    ESCAPE = SDLK_ESCAPE,
    BACKSPACE = SDLK_BACKSPACE,
    TAB = SDLK_TAB,
    SPACE = SDLK_SPACE,
    EXCLAIM = SDLK_EXCLAIM,
    QUOTEDBL = SDLK_QUOTEDBL,
    HASH = SDLK_HASH,
    PERCENT = SDLK_PERCENT,
    DOLLAR = SDLK_DOLLAR,
    AMPERSAND = SDLK_AMPERSAND,
    QUOTE = SDLK_QUOTE,
    LEFTPAREN = SDLK_LEFTPAREN,
    RIGHTPAREN = SDLK_RIGHTPAREN,
    ASTERISK = SDLK_ASTERISK,
    PLUS = SDLK_PLUS,
    COMMA = SDLK_COMMA,
    MINUS = SDLK_MINUS,
    PERIOD = SDLK_PERIOD,
    SLASH = SDLK_SLASH,
    NUM_0 = SDLK_0,
    NUM_1 = SDLK_1,
    NUM_2 = SDLK_2,
    NUM_3 = SDLK_3,
    NUM_4 = SDLK_4,
    NUM_5 = SDLK_5,
    NUM_6 = SDLK_6,
    NUM_7 = SDLK_7,
    NUM_8 = SDLK_8,
    NUM_9 = SDLK_9,
    COLON = SDLK_COLON,
    SEMICOLON = SDLK_SEMICOLON,
    LESS = SDLK_LESS,
    EQUALS = SDLK_EQUALS,
    GREATER = SDLK_GREATER,
    QUESTION = SDLK_QUESTION,
    AT = SDLK_AT,
    LEFTBRACKET = SDLK_LEFTBRACKET,
    BACKSLASH = SDLK_BACKSLASH,
    RIGHTBRACKET = SDLK_RIGHTBRACKET,
    CARET = SDLK_CARET,
    UNDERSCORE = SDLK_UNDERSCORE,
    BACKQUOTE = SDLK_BACKQUOTE,
    A = SDLK_a,
    B = SDLK_b,
    C = SDLK_c,
    D = SDLK_d,
    E = SDLK_e,
    F = SDLK_f,
    G = SDLK_g,
    H = SDLK_h,
    I = SDLK_i,
    J = SDLK_j,
    K = SDLK_k,
    L = SDLK_l,
    M = SDLK_m,
    N = SDLK_n,
    O = SDLK_o,
    P = SDLK_p,
    Q = SDLK_q,
    R = SDLK_r,
    S = SDLK_s,
    T = SDLK_t,
    U = SDLK_u,
    V = SDLK_v,
    W = SDLK_w,
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
