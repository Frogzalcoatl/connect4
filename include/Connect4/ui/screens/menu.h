#pragma once
#include "Connect4/ui/button.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    SDL_Renderer* renderer;

    // [Owns] properties specific to this screen
    C4_TextUIElement* title;
    C4_Button* playButton;
    C4_Button* settingsButton;
    C4_Button* quitButton;
} C4_MenuScreen;

C4_MenuScreen* C4_MenuScreen_Create(SDL_Renderer* renderer);
void C4_MenuScreen_Destroy(C4_MenuScreen* screen);
void C4_MenuScreen_Draw(C4_MenuScreen* screen);
C4_ScreenChangeRequest C4_MenuScreen_HandleKeyboardInput(C4_MenuScreen* screen, SDL_Scancode scancode);
C4_ScreenChangeRequest C4_MenuScreen_HandleMouseEvents(C4_MenuScreen* screen, SDL_Event* event);