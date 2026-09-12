#pragma once
#include "Connect4/ui/themes.h"
#include "Connect4/ui/types.h"
#include <SDL3/SDL.h>

void C4_UI_DrawShape(
    SDL_FRect rect,
    C4_UI_Data_Shape* shape,
    C4_UI_StyleState* styleState,
    C4_UI_Mirror mirror,
    SDL_Renderer* renderer,
    float uiScale
);