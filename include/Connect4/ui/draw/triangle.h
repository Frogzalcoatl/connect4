#pragma once
#include "SDL3/SDL.h"
#include "Connect4/ui/node.h"

void C4_UI_DrawTriangle(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirror mirror, SDL_Renderer* renderer);
void C4_UI_DrawTriangleBorders(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirror mirror, SDL_Renderer* renderer);