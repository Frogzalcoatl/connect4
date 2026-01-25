#include "Connect4/ui/screens/game.h"
#include "Connect4/constants.h"
#include "Connect4/game/events.h"
#include "Connect4/game/consoleOutput.h"
#include <assert.h>

typedef struct {
    C4_Game* game;
} C4_GameScreenData;

static void C4_GameScreen_OnEnter(C4_UI_Screen* screen) {
    assert(screen && screen->data);
    (void)screen;
}

void C4_GameScreen_HandleWindowResize(C4_UI_Screen* screen) {
    assert(screen && screen->data);
    (void)screen;
    //C4_GameScreenData* gameData = (C4_GameScreenData*)screen->data;
    //C4_Game* game = gameData->game;
    //SDL_Renderer* renderer = game->renderer;
}

//static void C4_GameScreen_ResetGame(C4_GameScreenData* gameData) {
//    C4_Board_Reset(gameData->game->board);
//}

static void C4_GameScreen_Init(C4_UI_Screen* screen, C4_Game* game);

C4_UI_Screen* C4_GameScreen_Create(C4_Game* game) {
    assert(game);
    
    C4_UI_Screen* screen = C4_Screen_Create(game->renderer, game->textEngine);
    if (!screen) {
        C4_FatalError(C4_ErrorCode_OutOfMemory, "Unable to allocate memory for game screen");
    }

    screen->data = SDL_calloc(1, sizeof(C4_GameScreenData));
    if (!screen->data) {
        C4_FatalError(C4_ErrorCode_OutOfMemory, "Unable to allocate memory for game screen data");
    }

    screen->HandleWindowResize = C4_GameScreen_HandleWindowResize;
    screen->OnEnter = C4_GameScreen_OnEnter;

    C4_GameScreen_Init(screen, game);

    C4_Log("Created game screen");

    return screen;
}

void C4_GameScreen_Init(C4_UI_Screen* screen, C4_Game* game) {
    assert (screen && game && screen->data && game->renderer);
    
    //C4_UI_Canvas* canvas = &screen->canvas;
    C4_GameScreenData* data = (C4_GameScreenData*)screen->data;
    //SDL_Renderer* renderer = game->renderer;

    data->game = game;

    screen->HandleWindowResize(screen);
}