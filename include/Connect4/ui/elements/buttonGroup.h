#pragma once
#include "SDL3/SDL.h"
#include "Connect4/ui/elements/button.h"
#include <stdint.h>

typedef enum {
    C4_UI_ButtonGroup_Direction_Vertical,
    C4_UI_ButtonGroup_Direction_Horizontal
} C4_UI_ButtonGroup_Direction;

typedef struct {
    SDL_FRect destination;
    size_t count;
    C4_UI_ButtonGroup_Direction buttonDirection;
    unsigned int margin;
    TTF_Font* font;
    const C4_UI_Theme* theme;
} C4_UI_ButtonGroup_Config;

typedef struct {
    SDL_FRect bounds;
    C4_UI_Button* buttons;
    size_t count;
    C4_UI_ButtonGroup_Direction direction;
    unsigned int margin;
} C4_UI_ButtonGroup;

bool C4_UI_ButtonGroup_InitProperties(C4_UI_ButtonGroup* group, SDL_Renderer* renderer, const C4_UI_ButtonGroup_Config* config);
C4_UI_ButtonGroup* C4_UI_ButtonGroup_Create(SDL_Renderer* renderer, const C4_UI_ButtonGroup_Config* config);
void C4_UI_ButtonGroup_FreeResources(C4_UI_ButtonGroup* group);
void C4_UI_ButtonGroup_Destroy(void* data);
void C4_UI_ButtonGroup_Draw(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY);
void C4_UI_ButtonGroup_Update(void* data, float deltaTime);
void C4_UI_ButtonGroup_HandleMouseEvents(void* data, SDL_Event* event, float scale, float parentX, float parentY);
void C4_UI_ButtonGroup_ChangeDestination(C4_UI_ButtonGroup* group, const SDL_FRect rect);
void C4_UI_ButtonGroup_CenterInWindow(C4_UI_ButtonGroup* group, C4_Axis axis, unsigned int windowWidth, unsigned int windowHeight, float UIScale);
void C4_UI_ButtonGroup_Reset(void* data);
void C4_UI_ButtonGroup_ChangeDirection(C4_UI_ButtonGroup* group, C4_UI_ButtonGroup_Direction direction);
void C4_UI_ButtonGroup_ChangePtSize(C4_UI_ButtonGroup* group, float ptSize, SDL_Renderer* renderer);