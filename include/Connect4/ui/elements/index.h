#pragma once
#include "SDL3/SDL.h"
#include "Connect4/assets/fonts.h"

typedef enum {
    C4_Axis_X,
    C4_Axis_Y,
    C4_Axis_XY
} C4_Axis;

void C4_UI_CenterInWindow(SDL_FRect* rect, C4_Axis axis);