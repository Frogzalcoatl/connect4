#pragma once
#include "Connect4/ui/elements/index.h"
#include <SDL3/SDL.h>

typedef struct {
    SDL_FRect rectangle;
    SDL_Color color;
    unsigned int borderWidth;
} C4_UI_Borders;

bool C4_UI_Borders_InitProperties(C4_UI_Borders* element, const SDL_FRect rectangle, const SDL_Color color, const unsigned int borderWidth);
C4_UI_Borders* C4_UI_Borders_Create(const SDL_FRect rectangle, const SDL_Color color, const unsigned int borderWidth);
void C4_UI_Borders_Destroy(C4_UI_Borders* element);
void C4_UI_Borders_Draw(C4_UI_Borders* element, SDL_Renderer* renderer);