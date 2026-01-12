#include "Connect4/ui/screens/settings.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include <stdio.h>

typedef struct {
    C4_Game* game;
} C4_SettingsScreenData;

static void C4_SettingsScreen_OnEnter(C4_UI_Screen* screen) {
    if (!screen || !screen->data) {
        return;
    }
    //C4_SettingsScreenData* data = (C4_SettingsScreenData*)screen->data;
}

static void C4_SettingsScreen_HandleWindowResize(C4_UI_Screen* screen, C4_UI_LayoutType layout) {
    if (!screen || !screen->data) {
        return;
    }
    //C4_SettingsScreenData* data = (C4_SettingsScreenData*)screen->data;

    switch (layout) {
        case C4_UI_LayoutType_Wide: {
            
        }; break;
        case C4_UI_LayoutType_Tall: {

        }; break;
        default: break;
    }

}

static bool C4_SettingsScreen_Init(C4_UI_Screen* screen, C4_Game* game);

C4_UI_Screen* C4_SettingsScreen_Create(C4_Game* game) {
    if (!game) {
        return NULL;
    }
    C4_UI_Screen* screen = C4_Screen_Create(game->renderer, game->textEngine);
    if (!screen) {
        return NULL;
    }

    screen->data = calloc(1, sizeof(C4_SettingsScreenData));
    if (!screen->data) {
        screen->Destroy(screen);
        return NULL;
    }

    screen->HandleWindowResize = C4_SettingsScreen_HandleWindowResize;
    screen->OnEnter = C4_SettingsScreen_OnEnter;

    if (!C4_SettingsScreen_Init(screen, game)) {
        screen->Destroy(screen);
        return NULL;
    }

    return screen;
}

static bool C4_SettingsScreen_Init(C4_UI_Screen* screen, C4_Game* game) {
    if (!screen || !game || !screen->data || !game->renderer) {
        return false;
    }
    //C4_UI_Canvas* canvas = &screen->canvas;
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)screen->data;
    //SDL_Renderer* renderer = game->renderer;

    data->game = game;

    screen->HandleWindowResize(screen, game->currentLayout);

    return true;
}