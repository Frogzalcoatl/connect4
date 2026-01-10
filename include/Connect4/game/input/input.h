#pragma once
#include <SDL3/SDL.h>

typedef enum {
    C4_ACTION_NONE,
    C4_ACTION_NAV_UP,
    C4_ACTION_NAV_DOWN,
    C4_ACTION_NAV_RIGHT,
    C4_ACTION_NAV_LEFT,
    C4_ACTION_CONFIRM,
    C4_ACTION_CANCEL
} C4_InputAction;

void C4_Input_Shutdown(void);
C4_InputAction C4_GetInput(SDL_Event* event);