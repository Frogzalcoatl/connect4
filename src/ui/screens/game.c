#include "Connect4/ui/screens/game.h"
#include "Connect4/constants.h"
#include "Connect4/game/events.h"

typedef struct {
    C4_Game* game;
} C4_GameScreenData;

static void C4_GameScreen_OnEnter(C4_UI_Screen* screen) {
    if (!screen || !screen->data) {
        return;
    }
}

void C4_GameScreen_HandleWindowResize(C4_UI_Screen* screen, C4_UI_LayoutType layout) {
    if (!screen || !screen->data) {
        return;
    }
    C4_GameScreenData* gameData = (C4_GameScreenData*)screen->data;
    C4_Game* game = gameData->game;
    SDL_Renderer* renderer = game->renderer;

    switch (layout) {
        case C4_UI_LayoutType_Wide: {
            
        }; break;
        case C4_UI_LayoutType_Tall: {
            
        }; break;
        default: break;
    }
}

static void C4_GameScreen_ResetGame(C4_GameScreenData* gameData) {
    C4_Board_Reset(gameData->game->board);
}

static bool C4_GameScreen_Init(C4_UI_Screen* screen, C4_Game* game);

C4_UI_Screen* C4_GameScreen_Create(C4_Game* game) {
    if (!game) {
        return NULL;
    }
    C4_UI_Screen* screen = C4_Screen_Create(game->renderer, game->textEngine);
    if (!screen) {
        return NULL;
    }

    screen->data = calloc(1, sizeof(C4_GameScreenData));
    if (!screen->data) {
        screen->Destroy(screen);
        return NULL;
    }

    screen->HandleWindowResize = C4_GameScreen_HandleWindowResize;
    screen->OnEnter = C4_GameScreen_OnEnter;

    if (!C4_GameScreen_Init(screen, game)) {
        screen->Destroy(screen);
        return NULL;
    }

    return screen;
}

bool C4_GameScreen_Init(C4_UI_Screen* screen, C4_Game* game) {
    if (!screen || !game || !screen->data || !game->renderer) {
        return false;
    }
    C4_UI_Canvas* canvas = &screen->canvas;
    C4_GameScreenData* data = (C4_GameScreenData*)screen->data;
    SDL_Renderer* renderer = game->renderer;

    data->game = game;

    screen->HandleWindowResize(screen, game->currentLayout);

    return true;
}