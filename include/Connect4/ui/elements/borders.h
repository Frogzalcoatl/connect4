#pragma once
#include "Connect4/ui/elements/index.h"
#include "SDL3/SDL.h"

typedef struct {
    SDL_FRect destination;
    SDL_Color color;
    unsigned int width;
} C4_UI_Borders;

bool C4_UI_Borders_InitProperties(C4_UI_Borders* element, const SDL_FRect destination, const SDL_Color color, const unsigned int width);
C4_UI_Borders* C4_UI_Borders_Create(const SDL_FRect destination, const SDL_Color color, const unsigned int width);
void C4_UI_Borders_Destroy(void* data);
void C4_UI_Borders_Draw(void* data, SDL_Renderer* renderer);