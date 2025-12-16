#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Connect4/game/board.h"
#include "Connect4/ui/screens/interface.h"

bool Connect4_Init_Dependencies();
void Connect4_Quit_Dependencies();

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    C4_Board* board;
    C4_Screen_Interface currentScreen;
    bool running;
    bool isFullscreen;
} C4_Game;

C4_Game* C4_Game_Create(uint8_t boardWidth, uint8_t boardHeight, uint8_t amountToWin);
void C4_Game_Destroy(C4_Game* game);
void C4_Game_Run(C4_Game* game);