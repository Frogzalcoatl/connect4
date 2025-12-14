#pragma once
#include "Connect4/game/board.h"
#include "Connect4/ui/text_element.h"
#include "Connect4/ui/rectangle_element.h"
#include "Connect4/ui/button.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    C4_Board* board;
    SDL_Renderer* renderer;
    
    // [Owns] properties specific to this screen
    C4_UI_Text* testBoardText;
    C4_UI_Button* menuButton;
} C4_Screen_Game;

C4_Screen_Game* C4_GameScreen_Create(SDL_Renderer* renderer, C4_Board* board);
void C4_GameScreen_Destroy(void* screenData);
void C4_GameScreen_Draw(void* screenData);
void C4_GameScreen_TestStrUpdate(C4_Screen_Game* screen);
C4_Screen_RequestChange C4_GameScreen_HandleKeyboardInput(void* screenData, SDL_Scancode scancode);
C4_Screen_RequestChange C4_GameScreen_HandleMouseEvents(void* screenData, SDL_Event* event);