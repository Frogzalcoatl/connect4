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
    C4_TextUIElement* testBoardText;
    C4_Button* menuButton;
} C4_GameScreen;

C4_GameScreen* C4_GameScreen_Create(C4_Board* board, SDL_Renderer* renderer);
void C4_GameScreen_Destroy(C4_GameScreen* screen);
void C4_GameScreen_Draw(C4_GameScreen* screen);
void C4_GameScreen_TestStrUpdate(C4_GameScreen* screen);
C4_ScreenChangeRequest C4_GameScreen_HandleKeyboardInput(C4_GameScreen* screen, SDL_Scancode scancode);
C4_ScreenChangeRequest C4_GameScreen_HandleMouseEvents(C4_GameScreen* screen, SDL_Event* event);