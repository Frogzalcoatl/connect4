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
} C4_RectangleUIElement;

C4_RectangleUIElement* C4_RectangleUIElement_Create(const SDL_FRect rectangle, const SDL_Color color);
void C4_RectangleUIElement_Destroy(C4_RectangleUIElement* element);
void C4_RectangleUIElement_Draw(C4_RectangleUIElement* element, SDL_Renderer* renderer);
void C4_RectangleUIElement_CenterInWindow(C4_RectangleUIElement* element, C4_Axis axis);