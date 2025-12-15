#pragma once
#include "Connect4/ui/elements/borders.h"
#include "Connect4/ui/elements/button.h"
#include <stdbool.h>

typedef struct {
    SDL_Renderer* renderer;
    C4_UI_Button* button1;
    C4_UI_Button* button2;
    C4_UI_Borders* borders;
    C4_UI_Rectangle* background;
    C4_UI_Text* message;
    unsigned int borderWidth;
    unsigned int buttonHeight;
    bool isShowing;
} C4_UI_Popup;

C4_UI_Popup* C4_UI_Popup_Create(SDL_Renderer* renderer, SDL_FRect rectangle, unsigned int borderWidth, unsigned int buttonHeight, const char* button1Text, const char* button2Text, const char* messageText, float buttonPtSize, float textPtSize);
void C4_UI_Popup_Destroy(C4_UI_Popup* popup);
void C4_UI_Popup_Draw(C4_UI_Popup* popup);
void C4_UI_Popup_CenterInWindow(C4_UI_Popup* popup);
// returns 1 or 2 for its respective buttons. 0 if no button was pressed.
unsigned short C4_UI_Popup_HandleMouseEvents(C4_UI_Popup* popup, SDL_Event* event);