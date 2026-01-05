#pragma once
#include "Connect4/ui/elements/index.h"
#include "SDL3/SDL.h"

// Copy pasted config struct for the sake of consistency with other ui elements
typedef struct {
    SDL_FRect destination;
    SDL_Color color;
    unsigned int width;
} C4_UI_Borders_Config;

typedef struct {
    SDL_FRect destination;
    SDL_Color color;
    unsigned int width;
} C4_UI_Borders;

bool C4_UI_Borders_InitProperties(C4_UI_Borders* element, const C4_UI_Borders_Config* config);
C4_UI_Borders* C4_UI_Borders_Create(const C4_UI_Borders_Config* config);
void C4_UI_Borders_Destroy(void* data);
void C4_UI_Borders_Draw(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY);
void C4_UI_Borders_ChangeDestination(C4_UI_Borders* element, const SDL_FRect destination);