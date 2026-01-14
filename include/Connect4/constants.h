#pragma once
#include "SDL3/SDL.h"
#include "Connect4/ui/themes.h"

// Window
#define C4_WIDE_LAYOUT_BASE_WINDOW_WIDTH 1920
#define C4_WIDE_LAYOUT_BASE_WINDOW_HEIGHT 1080
#define C4_TALL_LAYOUT_BASE_WINDOW_WIDTH C4_WIDE_LAYOUT_BASE_WINDOW_HEIGHT
#define C4_TALL_LAYOUT_BASE_WINDOW_HEIGHT C4_WIDE_LAYOUT_BASE_WINDOW_WIDTH

//Theme
#define C4_WINDOW_BG_COLOR (SDL_Color){15, 20, 30, 255}

#define C4_UI_THEME_DEFAULT (C4_UI_Theme){\
    .style = (C4_UI_Style){\
        .normal = (C4_UI_StyleState){\
            .background = (SDL_Color){30, 41, 59, 255},\
            .text       = (SDL_Color){226, 232, 240, 255},\
            .border     = (SDL_Color){71, 85, 105, 255}\
        },\
        .hovered = (C4_UI_StyleState){\
            .background = (SDL_Color){51, 65, 85, 255},\
            .text       = (SDL_Color){255, 255, 255, 255},\
            .border     = (SDL_Color){148, 163, 184, 255}\
        },\
        .pressed = (C4_UI_StyleState){\
            .background = (SDL_Color){191, 219, 254, 255},\
            .text       = (SDL_Color){23, 37, 84, 255},\
            .border     = (SDL_Color){219, 234, 254, 255}\
        },\
        .inactive = (C4_UI_StyleState){\
            .background = (SDL_Color){15, 23, 42, 255},\
            .text       = (SDL_Color){100, 116, 139, 255},\
            .border     = (SDL_Color){30, 41, 59, 255}\
        }\
    },\
    .defaultPtSize = 32.f,\
    .borderWidth   = 3,\
    .textColor     = (SDL_Color){226, 232, 240, 255}\
}

#define DEFAULT_BUTTON_SIZE (SDL_FRect){0.f, 0.f, 800.f, 100.f}

// other
#define C4_EMPTY_SDL_FRECT (SDL_FRect){0.f, 0.f, 1.f, 1.f}
#define C4_DEFAULT_PT_SIZE_TALL_LAYOUT 75.f
#define SAHARS_RETROLINK_CONTROLLER_MAPPING "03000000790000000600000000000000,SaharsRetrolinkController,platform:Windows,a:b6,b:b8,start:b9,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,"