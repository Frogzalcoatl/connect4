#pragma once
#include "SDL3/SDL.h"

typedef struct C4_UI_StyleState {
    SDL_Color background;
    SDL_Color text;
    SDL_Color border;
} C4_UI_StyleState;

typedef struct C4_UI_Style {
    C4_UI_StyleState normal;
    C4_UI_StyleState hovered;
    C4_UI_StyleState pressed;
    C4_UI_StyleState inactive;
} C4_UI_Style;

typedef struct C4_UI_Theme {
    C4_UI_Style style;
    float defaultPtSize;
    unsigned int borderWidth;
    SDL_Color textColor;
} C4_UI_Theme;