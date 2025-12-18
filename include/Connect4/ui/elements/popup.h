#pragma once
#include "Connect4/ui/elements/borders.h"
#include "Connect4/ui/elements/buttonGroup.h"
#include <stdbool.h>

typedef struct {
    SDL_Renderer* renderer;
    C4_UI_ButtonGroup buttonGroup;
    C4_UI_Borders borders;
    C4_UI_Rectangle background;
    C4_UI_Text message;
    bool isShowing;
} C4_UI_Popup;

bool C4_UI_Popup_InitProperties(
    C4_UI_Popup* popup, SDL_Renderer* renderer, SDL_FRect rectangle, unsigned int borderWidth, C4_UI_ButtonGroup_Direction buttonDirection,
    size_t buttonCount, float buttonGroupHeight, const char* messageText, float buttonPtSize, float messagePtSize
);
C4_UI_Popup* C4_UI_Popup_Create(
    SDL_Renderer* renderer, SDL_FRect rectangle, unsigned int borderWidth, C4_UI_ButtonGroup_Direction buttonDirection,
    size_t buttonCount, float buttonGroupHeight, const char* messageText, float buttonPtSize, float messagePtSize
);
void C4_UI_Popup_FreeResources(C4_UI_Popup* popup);
void C4_UI_Popup_Destroy(C4_UI_Popup* popup);
void C4_UI_Popup_Draw(C4_UI_Popup* popup);
void C4_UI_Popup_CenterInWindow(C4_UI_Popup* popup);
int C4_UI_Popup_HandleMouseEvents(C4_UI_Popup* popup, SDL_Event* event);