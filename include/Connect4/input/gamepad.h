#pragma once
#include <SDL3/SDL.h>

#define C4_MAX_GAMEPADS 8

typedef enum C4_InputVerb {
    C4_INPUT_VERB_NONE,
    C4_INPUT_VERB_NAV_UP,
    C4_INPUT_VERB_NAV_DOWN,
    C4_INPUT_VERB_NAV_RIGHT,
    C4_INPUT_VERB_NAV_LEFT,
    C4_INPUT_VERB_CONFIRM,
    C4_INPUT_VERB_CANCEL,
    C4_INPUT_VERB_COUNT
} C4_InputVerb;

typedef struct C4_VerbMapping {
    SDL_Scancode* scancodes;
    int count;
    int capacity;
} C4_VerbMapping;

typedef enum C4_InputState {
    C4_INPUT_STATE_PRESSED,
    C4_INPUT_STATE_RELEASED
} C4_InputState;

typedef struct C4_InputEvent {
    C4_InputVerb verb;
    C4_InputState state;
} C4_InputEvent;

void C4_Input_Init(void);
void C4_Input_Shutdown(void);
C4_InputEvent C4_GetInput(SDL_Event* event);
void C4_Gamepad_SetActiveIndex(int index);
void C4_Gamepad_GetNames(char** returnValue, size_t returnValueSize);
void C4_Gamepad_GetActiveName(char* returnValue, size_t returnValueSize);
void C4_Gamepad_OnlyAcceptInputFromActiveGamepad(bool value);
void C4_Input_ConnectScancodeToVerb(C4_InputVerb inputVerb, SDL_Scancode scancode);
void C4_Input_DisconnectScancodeFromVerb(C4_InputVerb inputVerb, SDL_Scancode scancode);
bool C4_Input_CheckRepeat(float deltaTime, C4_InputEvent* outEvent);
void C4_Input_ResetVerbState(C4_InputVerb verb);