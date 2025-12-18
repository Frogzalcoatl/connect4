#pragma once
#include <SDL3/SDL.h>
#include "Connect4/ui/elements/index.h"

typedef struct {
    SDL_FRect destination;
    SDL_Color color;
} C4_UI_Rectangle;

bool C4_UI_Rectangle_InitProperties(C4_UI_Rectangle* element, const SDL_FRect destination, const SDL_Color color);
C4_UI_Rectangle* C4_UI_Rectangle_Create(const SDL_FRect destination, const SDL_Color color);
void C4_UI_Rectangle_Destroy(C4_UI_Rectangle* element);
void C4_UI_Rectangle_Draw(C4_UI_Rectangle* element, SDL_Renderer* renderer);