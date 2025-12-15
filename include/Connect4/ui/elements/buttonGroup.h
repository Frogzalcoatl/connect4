#pragma once
#include <SDL3/SDL.h>
#include "Connect4/ui/elements/button.h"
#include <stdint.h>

typedef enum {
    C4_UI_ButtonGroup_Direction_Vertical,
    C4_UI_ButtonGroup_Direction_Horizontal
} C4_UI_ButtonGroup_Direction;

typedef struct {
    SDL_FRect bounds;
    C4_UI_Button* buttons;
    size_t count;
    C4_UI_ButtonGroup_Direction direction;
    unsigned int margin;
} C4_UI_ButtonGroup;

bool C4_UI_ButtonGroup_InitProperties(C4_UI_ButtonGroup* stack, SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonGroup_Direction direction, unsigned int margin, float buttonPtSize);
C4_UI_ButtonGroup* C4_UI_ButtonGroup_Create(SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonGroup_Direction direction, unsigned int margin, float buttonPtSize);
void C4_UI_ButtonGroup_FreeResources(C4_UI_ButtonGroup* stack);
void C4_UI_ButtonGroup_Destroy(C4_UI_ButtonGroup* stack);
void C4_UI_ButtonGroup_SetButtonIndex(C4_UI_ButtonGroup* stack, size_t buttonIndex, SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize);
void C4_UI_ButtonGroup_Draw(C4_UI_ButtonGroup* stack, SDL_Renderer* renderer);
long long C4_UI_ButtonGroup_HandleMouseEvents(C4_UI_ButtonGroup* stack, SDL_Event* event, SDL_Renderer* renderer);
void C4_UI_ButtonGroup_TransformResize(C4_UI_ButtonGroup* stack, const SDL_FRect rect);
void C4_UI_ButtonGroup_CenterInWindow(C4_UI_ButtonGroup* stack, C4_Axis axis);