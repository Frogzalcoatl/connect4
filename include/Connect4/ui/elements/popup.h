#pragma once
#include "Connect4/ui/elements/borders.h"
#include "Connect4/ui/elements/buttonGroup.h"
#include <stdbool.h>

typedef struct {
    C4_UI_ButtonGroup buttonGroup;
    C4_UI_Borders borders;
    C4_UI_Rectangle background;
    C4_UI_Text message;
    bool isShowing;
} C4_UI_Popup;

bool C4_UI_Popup_InitProperties(
    C4_UI_Popup* popup, SDL_Renderer* renderer, const SDL_FRect destination, C4_UI_ButtonGroup_Direction buttonDirection,
    size_t buttonCount, float buttonGroupHeight, const char* messageText, const C4_UI_Theme* theme
);
C4_UI_Popup* C4_UI_Popup_Create(
    SDL_Renderer* renderer, const SDL_FRect destination, C4_UI_ButtonGroup_Direction buttonDirection, size_t buttonCount, 
    float buttonGroupHeight, const char* messageText, const C4_UI_Theme* theme
);
void C4_UI_Popup_FreeResources(C4_UI_Popup* popup);
void C4_UI_Popup_Destroy(C4_UI_Popup* popup);
void C4_UI_Popup_Draw(C4_UI_Popup* popup, SDL_Renderer* renderer);
void C4_UI_Popup_CenterInWindow(C4_UI_Popup* popup, SDL_Renderer* renderer);
int C4_UI_Popup_HandleMouseEvents(C4_UI_Popup* popup, SDL_Event* event);