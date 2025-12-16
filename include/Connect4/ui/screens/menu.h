#pragma once
#include "Connect4/ui/elements/buttonGroup.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    SDL_Renderer* renderer;

    // [Owns] properties specific to this screen
    C4_UI_Text title;
    C4_UI_ButtonGroup buttonGroup;
} C4_Screen_Menu;

C4_Screen_Menu* C4_Screen_Menu_Create(SDL_Renderer* renderer);
void C4_Screen_Menu_Destroy(void* screenData);
void C4_Screen_Menu_Draw(void* screenData);
void C4_Screen_Menu_HandleKeyboardInput(void* screenData, SDL_Scancode scancode);
void C4_Screen_Menu_HandleMouseEvents(void* screenData, SDL_Event* event);