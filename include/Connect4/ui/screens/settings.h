#pragma once
#include "Connect4/ui/elements/buttonGroup.h"
#include "Connect4/ui/elements/popup.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    SDL_Renderer* renderer;
    SDL_Window* window;

    // [Owns] properties specific to this screen
    C4_UI_Text title;
    C4_UI_ButtonGroup buttonGroup;
    C4_UI_Popup confirmationPopup;
} C4_Screen_Settings;

C4_Screen_Settings* C4_Screen_Settings_Create(SDL_Renderer* renderer, SDL_Window* window);
void C4_Screen_Settings_Destroy(void* screenData);
void C4_Screen_Settings_Draw(void* screenData);
C4_Screen_RequestChange C4_Screen_Settings_HandleKeyboardInput(void* screenData, SDL_Scancode scancode);
C4_Screen_RequestChange C4_Screen_Settings_HandleMouseEvents(void* screenData, SDL_Event* event);