#pragma once
#include <SDL3/SDL.h>
#include "Connect4/ui/elements/button.h"
#include <stdint.h>

typedef enum {
    C4_UI_ButtonStack_Direction_Vertical,
    C4_UI_ButtonStack_Direction_Horizontal
} C4_UI_ButtonStack_Direction;

typedef struct {
    SDL_FRect bounds;
    C4_UI_Button* buttons;
    size_t count;
    C4_UI_ButtonStack_Direction direction;
    unsigned int margin;
} C4_UI_ButtonStack;

bool C4_UI_ButtonStack_InitProperties(C4_UI_ButtonStack* stack, SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonStack_Direction direction, unsigned int margin, float buttonPtSize);
C4_UI_ButtonStack* C4_UI_ButtonStack_Create(SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonStack_Direction direction, unsigned int margin, float buttonPtSize);
void C4_UI_ButtonStack_FreeResources(C4_UI_ButtonStack* stack);
void C4_UI_ButtonStack_Destroy(C4_UI_ButtonStack* stack);
void C4_UI_ButtonStack_SetButtonIndex(C4_UI_ButtonStack* stack, size_t buttonIndex, SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize);
void C4_UI_ButtonStack_Draw(C4_UI_ButtonStack* stack, SDL_Renderer* renderer);
long long C4_UI_ButtonStack_HandleMouseEvents(C4_UI_ButtonStack* stack, SDL_Event* event, SDL_Renderer* renderer);
void C4_UI_ButtonStack_TransformResize(C4_UI_ButtonStack* stack, const SDL_FRect rect);
void C4_UI_ButtonStack_CenterInWindow(C4_UI_ButtonStack* stack, C4_Axis axis);