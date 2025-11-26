#pragma once
#include "Connect4/ui/button.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    SDL_Renderer* renderer;
    SDL_Window* window;

    // [Owns] properties specific to this screen
    C4_TextUIElement* title;
    C4_Button* backButton;
} C4_SettingsScreen;

C4_SettingsScreen* C4_SettingsScreen_Create(SDL_Window* window, SDL_Renderer* renderer);
void C4_SettingsScreen_Destroy(C4_SettingsScreen* screen);
void C4_SettingsScreen_Draw(C4_SettingsScreen* screen);
C4_ScreenChangeRequest C4_SettingsScreen_HandleKeyboardInput(C4_SettingsScreen* screen, SDL_Scancode scancode);
C4_ScreenChangeRequest C4_SettingsScreen_HandleMouseEvents(C4_SettingsScreen* screen, SDL_Event* event);