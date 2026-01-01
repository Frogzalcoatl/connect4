#pragma once
#include "Connect4/ui/elements/borders.h"
#include "Connect4/ui/elements/buttonGroup.h"
#include <stdbool.h>

typedef struct {
    SDL_FRect destination;
    unsigned int min;
    unsigned int max;
    unsigned int startingValue;
    TTF_Font* font;
    const C4_UI_Theme* theme;
} C4_UI_NumberInput_Config;

typedef struct {
    C4_UI_Borders borders;
    C4_UI_Rectangle background;
    C4_UI_ButtonGroup buttonGroup;
    C4_UI_Text numberText;
    unsigned int min;
    unsigned int max;
    unsigned int currentValue;
} C4_UI_NumberInput;

bool C4_UI_NumberInput_InitProperties(
    C4_UI_NumberInput* numInput, SDL_Renderer* renderer, const C4_UI_NumberInput_Config* config
);
C4_UI_NumberInput* C4_UI_NumberInput_Create(
    SDL_Renderer* renderer, const C4_UI_NumberInput_Config* config
);
void C4_UI_NumberInput_FreeResources(C4_UI_NumberInput* numInput);
void C4_UI_NumberInput_Destroy(void* data);
void C4_UI_NumberInput_Draw(void* data, SDL_Renderer* renderer);
void C4_UI_NumberInput_CenterInWindow(C4_UI_NumberInput* numInput, C4_Axis axis, unsigned int windowWidth, unsigned int windowHeight);
void C4_UI_NumberInput_Update(void* data, float deltaTime);
void C4_UI_NumberInput_HandleMouseEvents(void* data, SDL_Event* event);
void C4_UI_NumberInput_HandleKeyboardInput(C4_UI_NumberInput* numInput, SDL_Event* event, SDL_Renderer* renderer);
void C4_UI_NumberInput_ChangeMax(C4_UI_NumberInput* numInput, unsigned int newMax, SDL_Renderer* renderer);
void C4_UI_NumberInput_ChangeMin(C4_UI_NumberInput* numInput, unsigned int newMin, SDL_Renderer* renderer);
void C4_UI_NumberInput_GenericIncrementCallback(void* numberInputContext);
void C4_UI_NumberInput_GenericDecrementCallback(void* numberInputContext);
void C4_UI_NumberInput_ResetButtons(void* data);