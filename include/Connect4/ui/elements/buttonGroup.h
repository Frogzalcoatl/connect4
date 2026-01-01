#pragma once
#include "SDL3/SDL.h"
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

bool C4_UI_ButtonGroup_InitProperties(C4_UI_ButtonGroup* group, SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonGroup_Direction direction, unsigned int margin, const C4_UI_Theme* theme);
C4_UI_ButtonGroup* C4_UI_ButtonGroup_Create(SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonGroup_Direction direction, unsigned int margin, const C4_UI_Theme* theme);
void C4_UI_ButtonGroup_FreeResources(C4_UI_ButtonGroup* group);
void C4_UI_ButtonGroup_Destroy(void* data);
void C4_UI_ButtonGroup_SetButtonIndex(
    C4_UI_ButtonGroup* group, size_t buttonIndex, SDL_Renderer* renderer, const char* str, C4_UI_SymbolType symbol,
    float symbolWidth, float symbolHeight, int symbolRotationDegrees, const C4_UI_Theme* theme, C4_UI_Callback callback, void* callbackContext
);
void C4_UI_ButtonGroup_Draw(void* data, SDL_Renderer* renderer);
void C4_UI_ButtonGroup_HandleMouseEvents(void* data, SDL_Event* event);
void C4_UI_ButtonGroup_TransformResize(C4_UI_ButtonGroup* group, const SDL_FRect rect);
void C4_UI_ButtonGroup_CenterInWindow(C4_UI_ButtonGroup* group, C4_Axis axis);
void C4_UI_ButtonGroup_Reset(void* data);