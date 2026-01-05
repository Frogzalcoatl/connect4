#include <SDL3/SDL.h>
#include "Connect4/ui/elements/popup.h"
#pragma once

typedef enum {
    C4_ScreenType_Menu = 1,
    C4_ScreenType_Game,
    C4_ScreenType_Settings,
    C4_ScreenType_ScreenCount
} C4_ScreenType;

typedef struct {
    float buttonGroupHeight;
    float buttonGroupPtSize;
    float messagePtSize;
    C4_UI_ButtonGroup_Direction direction;
    SDL_FRect destination;
    SDL_Renderer* renderer;
} C4_Screen_UpdatePopup_Config;
void C4_Screen_UpdatePopup(C4_UI_Popup* popup, C4_Screen_UpdatePopup_Config* config);