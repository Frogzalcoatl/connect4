#pragma once
#include "SDL3/SDL.h"
#include "Connect4/ui/elements/index.h"

typedef struct {
    SDL_FRect destination;
    SDL_Color color;
} C4_UI_Rectangle_Config;

typedef struct {
    SDL_FRect destination;
    SDL_Color color;
} C4_UI_Rectangle;

bool C4_UI_Rectangle_InitProperties(C4_UI_Rectangle* element, const C4_UI_Rectangle_Config* config);
C4_UI_Rectangle* C4_UI_Rectangle_Create(const C4_UI_Rectangle_Config* config);
void C4_UI_Rectangle_Destroy(void* data);
void C4_UI_Rectangle_Draw(void* data, SDL_Renderer* renderer);