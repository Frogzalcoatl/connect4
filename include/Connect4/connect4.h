#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Connect4/game/board.h"
#include "Connect4/ui/text_element.h"
#include "Connect4/ui/screens/game.h"
#include "Connect4/ui/screens/menu.h"
#include "Connect4/ui/screens/settings.h"

bool Connect4_Init_Dependencies();
void Connect4_Quit_Dependencies();

typedef enum {
    C4_ScreenType_Menu,
    C4_ScreenType_Game,
    C4_ScreenType_Settings
} ScreenType;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    C4_Board board;
    C4_GameScreen* gameScreen;
    C4_MenuScreen* menuScreen;
    C4_SettingsScreen* settingsScreen;
    ScreenType currentScreen;
    bool running;
    bool isFullscreen;
} C4_Game;

C4_Game* C4_Game_Create(uint8_t boardWidth, uint8_t boardHeight);
void C4_Game_Destroy(C4_Game* game);
void C4_Game_Run(C4_Game* game);