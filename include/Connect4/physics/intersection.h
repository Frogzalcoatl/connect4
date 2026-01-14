#pragma once
#include "SDL3/SDL.h"
#include "Connect4/ui/types.h"

bool C4_IsPointInsideShape(C4_UI_ShapeType shapeType, SDL_FPoint point, SDL_FRect shapeBounds, float rotationDegrees, C4_UI_Mirroring mirror);
bool C4_IsPointInsideRectangle(SDL_FPoint point, SDL_FRect rectangleBounds, float rotationDegrees, C4_UI_Mirroring mirror);
bool C4_IsPointInsideTriangle(SDL_FPoint point, SDL_FRect triangleBounds, float rotationDegrees, C4_UI_Mirroring mirror);
bool C4_IsPointInsideEllipse(SDL_FPoint point, SDL_FRect circleBounds, float rotationDegrees, C4_UI_Mirroring mirror);