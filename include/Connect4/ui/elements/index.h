#pragma once
#include "SDL3/SDL.h"

typedef enum {
    C4_Axis_X,
    C4_Axis_Y,
    C4_Axis_XY
} C4_Axis;

void C4_UI_ClampToWindow(SDL_FRect* rect, unsigned int windowWidth, unsigned int windowHeight, float UIScale);
void C4_UI_CenterInWindow(SDL_FRect* rect, C4_Axis axis, unsigned int windowWidth, unsigned int windowHeight, float UIScale);
void C4_UI_GetScaledRect (
    const SDL_FRect* baseRect, SDL_FRect* returnedRect, float scale, float parentX, float parentY
);

typedef enum {
    C4_UI_LayoutType_None,
    C4_UI_LayoutType_Wide,
    C4_UI_LayoutType_Tall
} C4_UI_LayoutType;

C4_UI_LayoutType C4_UI_GetCurrentLayout(unsigned int windowWidth, unsigned int windowHeight);