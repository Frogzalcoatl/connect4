#pragma once
#include "Connect4/ui/button.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    SDL_Renderer* renderer;
    SDL_Window* window;

    // [Owns] properties specific to this screen
    C4_UI_Text* title;
    C4_UI_Button* backButton;
} C4_Screen_Settings;

C4_Screen_Settings* C4_SettingsScreen_Create(SDL_Renderer* renderer, SDL_Window* window);
void C4_SettingsScreen_Destroy(void* screenData);
void C4_SettingsScreen_Draw(void* screenData);
C4_Screen_RequestChange C4_SettingsScreen_HandleKeyboardInput(void* screenData, SDL_Scancode scancode);
C4_Screen_RequestChange C4_SettingsScreen_HandleMouseEvents(void* screenData, SDL_Event* event);