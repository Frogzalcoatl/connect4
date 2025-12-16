#pragma once
#include "Connect4/game/board.h"
#include "Connect4/ui/elements/text.h"
#include "Connect4/ui/elements/rectangle.h"
#include "Connect4/ui/elements/button.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    C4_Board* board;
    SDL_Renderer* renderer;
    
    // [Owns] properties specific to this screen
    C4_UI_Text testBoardText;
    C4_UI_Button menuButton;
} C4_Screen_Game;

C4_Screen_Game* C4_Screen_Game_Create(SDL_Renderer* renderer, C4_Board* board);
void C4_Screen_Game_Destroy(void* screenData);
void C4_Screen_Game_Draw(void* screenData);
void C4_Screen_Game_TestStrUpdate(C4_Screen_Game* screen);
void C4_Screen_Game_HandleKeyboardInput(void* screenData, SDL_Scancode scancode);
void C4_Screen_Game_HandleMouseEvents(void* screenData, SDL_Event* event);