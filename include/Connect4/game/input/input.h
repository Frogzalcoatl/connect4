#pragma once
#include <SDL3/SDL.h>

#define C4_MAX_GAMEPADS 8

typedef enum {
    C4_INPUT_NONE,
    C4_INPUT_NAV_UP,
    C4_INPUT_NAV_DOWN,
    C4_INPUT_NAV_RIGHT,
    C4_INPUT_NAV_LEFT,
    C4_INPUT_CONFIRM,
    C4_INPUT_CANCEL
} C4_InputVerb;

typedef enum {
    C4_INPUT_STATE_PRESSED,
    C4_INPUT_STATE_RELEASED
} C4_InputState;

typedef struct {
    C4_InputVerb verb;
    C4_InputState state;
} C4_InputEvent;

void C4_Input_Shutdown(void);
C4_InputEvent C4_GetInput(SDL_Event* event);
void C4_Gamepad_SetActiveIndex(int index);
void C4_Gamepad_GetNames(char** returnValue, size_t returnValueSize);
void C4_Gamepad_GetActiveName(char* returnValue, size_t returnValueSize);
void C4_Gamepad_OnlyAcceptInputFromActiveGamepad(bool value);