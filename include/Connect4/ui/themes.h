#pragma once
#include "SDL3/SDL.h"
#include "Connect4/assets/fonts.h"

typedef struct {
    SDL_Color background;
    SDL_Color text;
    SDL_Color borders;
    SDL_Color symbol;
} C4_UI_Button_ColorInfo;

typedef struct {
    C4_UI_Button_ColorInfo buttonDefault;
    C4_UI_Button_ColorInfo buttonHovered;
    C4_UI_Button_ColorInfo buttonPressed;
    C4_UI_Button_ColorInfo buttonInactive;
    C4_FontType buttonFont;
    float defaultPtSize;
    unsigned int borderWidth;
    SDL_Color textColor;
} C4_UI_Theme;