#pragma once
#include "Connect4/ui/button.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    SDL_Renderer* renderer;

    // [Owns] properties specific to this screen
    C4_UI_Text* title;
    C4_UI_Button* playButton;
    C4_UI_Button* settingsButton;
    C4_UI_Button* quitButton;
} C4_Screen_Menu;

C4_Screen_Menu* C4_MenuScreen_Create(SDL_Renderer* renderer);
void C4_MenuScreen_Destroy(void* screenData);
void C4_MenuScreen_Draw(void* screenData);
C4_Screen_RequestChange C4_MenuScreen_HandleKeyboardInput(void* screenData, SDL_Scancode scancode);
C4_Screen_RequestChange C4_MenuScreen_HandleMouseEvents(void* screenData, SDL_Event* event);