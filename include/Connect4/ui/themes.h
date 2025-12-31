#pragma once
#include "SDL3/SDL.h"
#include "Connect4/assets/fonts.h"

typedef struct {
    SDL_Color background;
    SDL_Color text;
    SDL_Color borders;
    SDL_Color symbol;
} C4_UI_ButtonColorInfo;

typedef struct {
    C4_UI_ButtonColorInfo buttonDefault;
    C4_UI_ButtonColorInfo buttonHovered;
    C4_UI_ButtonColorInfo buttonPressed;
    C4_FontType buttonFont;
    float defaultPtSize;
    unsigned int borderWidth;
    SDL_Color textColor;
} C4_UI_Theme;