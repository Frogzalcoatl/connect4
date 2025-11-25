#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Connect4/game/board.h"

bool Connect4_Init_Dependencies();
void Connect4_Quit_Dependencies();

typedef struct {
    Board board;
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font_regular;
    TTF_Font* font_bold;
    char testStr[100];
} Connect4;

Connect4* Connect4_create(uint8_t boardWidth, uint8_t boardHeight);
void Connect4_destroy(Connect4* connect4);
void Connect4_run(Connect4* connect4);
void Connect4_drawTestStr(Connect4* connect4);