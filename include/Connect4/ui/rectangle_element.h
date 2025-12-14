#pragma once
#include <SDL3/SDL.h>

typedef enum {
    C4_Axis_X,
    C4_Axis_Y,
    C4_Axis_XY
} C4_Axis;

typedef struct {
    SDL_FRect rectangle;
    SDL_Color color;
} C4_UI_Rectangle;

C4_UI_Rectangle* C4_UI_Rectangle_Create(const SDL_FRect rectangle, const SDL_Color color);
void C4_UI_Rectangle_Destroy(C4_UI_Rectangle* element);
void C4_UI_Rectangle_Draw(C4_UI_Rectangle* element, SDL_Renderer* renderer);
void C4_UI_Rectangle_CenterInWindow(C4_UI_Rectangle* element, C4_Axis axis);