#pragma once
#include "SDL3/SDL.h"
#include <SDL3_ttf/SDL_ttf.h>
#include "Connect4/game/board.h"
#include "Connect4/ui/canvas.h"
#include "Connect4/ui/screens/screen.h"

bool Connect4_Init_Dependencies(void);
void Connect4_Quit_Dependencies(void);

typedef struct C4_Game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_TextEngine* textEngine;
    C4_Board* board;
    C4_UI_Screen* screens[C4_ScreenType_ScreenCount];
    C4_UI_Screen* currentScreen;
    C4_ScreenType currentScreenType;
    bool running;
    bool isFullscreen;
    float userScalePreference;
    float UIScale;
    C4_UI_LayoutType currentLayout;
    unsigned int windowWidth;
    unsigned int windowHeight;
    C4_SoundSystem* soundSystem;
} C4_Game;

C4_Game* C4_Game_Create(uint8_t boardWidth, uint8_t boardHeight, uint8_t amountToWin);
void C4_Game_Destroy(C4_Game* game);
void C4_Game_Run(C4_Game* game);

SDL_FPoint C4_GetReferenceWindowDimensions(unsigned int w, unsigned int h, float UIScale);