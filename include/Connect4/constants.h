#pragma once
#include "SDL3/SDL.h"
#include "Connect4/ui/themes.h"

// Window
#define C4_WIDE_LAYOUT_BASE_WINDOW_WIDTH 1920
#define C4_WIDE_LAYOUT_BASE_WINDOW_HEIGHT 1080
#define C4_TALL_LAYOUT_BASE_WINDOW_WIDTH C4_WIDE_LAYOUT_BASE_WINDOW_HEIGHT
#define C4_TALL_LAYOUT_BASE_WINDOW_HEIGHT C4_WIDE_LAYOUT_BASE_WINDOW_WIDTH

//Theme
#define C4_UI_THEME_DEFAULT (C4_UI_Theme){\
    .buttonDefault = (C4_UI_Button_ColorInfo){\
        .background = (SDL_Color){45, 45, 48, 255},\
        .text = (SDL_Color){230, 230, 230, 255},\
        .borders = (SDL_Color){80, 80, 80, 255},\
        .symbol = (SDL_Color){230, 230, 230, 255}\
    },\
    .buttonHovered = (C4_UI_Button_ColorInfo){\
        .background = (SDL_Color){70, 70, 75, 255},\
        .text = (SDL_Color){255, 255, 255, 255},\
        .borders = (SDL_Color){120, 120, 120, 255},\
        .symbol = (SDL_Color){255, 255, 255, 255}\
    },\
    .buttonPressed = (C4_UI_Button_ColorInfo){\
        .background = (SDL_Color){240, 240, 240, 255},\
        .text = (SDL_Color){20, 20, 20, 255},\
        .borders = (SDL_Color){0, 0, 0, 255},\
        .symbol = (SDL_Color){20, 20, 20, 255}\
    },\
    .buttonInactive = (C4_UI_Button_ColorInfo){\
    .background = (SDL_Color){40, 40, 40, 255},\
    .text       = (SDL_Color){110, 110, 110, 255},\
    .borders    = (SDL_Color){60, 60, 60, 255}, \
    .symbol     = (SDL_Color){110, 110, 110, 255}\
    },\
    .defaultPtSize = 50.f,\
    .borderWidth = 3,\
    .textColor = (SDL_Color){230, 230, 230, 255}\
}

// other
#define C4_EMPTY_SDL_FRECT (SDL_FRect){0.f, 0.f, 1.f, 1.f}
#define C4_DEFAULT_PT_SIZE_TALL_LAYOUT 75.f