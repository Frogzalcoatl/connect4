#pragma once
#include "SDL3/SDL.h"
#include <math.h>
#include "Connect4/ui/types.h"

SDL_FPoint C4_UI_RotatePoint(SDL_FPoint point, SDL_FPoint center, float degrees);
SDL_FPoint C4_UI_MirrorPoint(SDL_FPoint point, SDL_FPoint center, C4_UI_Mirror mirror);
float C4_UI_GetMirroredRotation(float degrees, C4_UI_Mirror mirror);
void C4_ColorToFColor(SDL_Color* color, SDL_FColor* fColor);
void C4_InitVertices(SDL_Vertex* vertices, size_t vertexCount, SDL_FColor fColor);
SDL_FPoint C4_GetRectCenter(SDL_FRect rect);