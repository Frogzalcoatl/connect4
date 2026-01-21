#include "Connect4/ui/screens/settings.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include "Connect4/game/consoleOutput.h"
#include <assert.h>
#include <stdio.h>

typedef struct {
    C4_Game* game;
} C4_SettingsScreenData;

static void C4_SettingsScreen_OnEnter(C4_UI_Screen* screen) {
    assert(screen && screen->data);
    //C4_SettingsScreenData* data = (C4_SettingsScreenData*)screen->data;
}

static void C4_SettingsScreen_HandleWindowResize(C4_UI_Screen* screen, C4_UI_LayoutType layout) {
    assert(screen && screen->data);
    assert(layout > C4_UI_LayoutType_None && layout < C4_UI_LayoutType_Count);
    //C4_SettingsScreenData* data = (C4_SettingsScreenData*)screen->data;

    switch (layout) {
        case C4_UI_LayoutType_Wide: {
            
        }; break;
        case C4_UI_LayoutType_Tall: {

        }; break;
        default: break;
    }

}

static void C4_SettingsScreen_Init(C4_UI_Screen* screen, C4_Game* game);

C4_UI_Screen* C4_SettingsScreen_Create(C4_Game* game) {
    assert(game);

    C4_UI_Screen* screen = C4_Screen_Create(game->renderer, game->textEngine);
    if (!screen) {
        C4_FatalError(C4_ErrorCode_OutOfMemory, "Unable to allocate memory for settings screen");
    }

    screen->data = SDL_calloc(1, sizeof(C4_SettingsScreenData));
    if (!screen->data) {
        C4_FatalError(C4_ErrorCode_OutOfMemory, "Unable to allocate memory for settings screen data");
    }

    screen->HandleWindowResize = C4_SettingsScreen_HandleWindowResize;
    screen->OnEnter = C4_SettingsScreen_OnEnter;

    C4_SettingsScreen_Init(screen, game);

    C4_Log("Created settings screen");

    return screen;
}

static void C4_SettingsScreen_Init(C4_UI_Screen* screen, C4_Game* game) {
    assert(screen && screen->data && game && game->renderer);

    //C4_UI_Canvas* canvas = &screen->canvas;
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)screen->data;
    //SDL_Renderer* renderer = game->renderer;

    data->game = game;

    screen->HandleWindowResize(screen, game->currentLayout);
}