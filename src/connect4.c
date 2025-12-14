#include "Connect4/connect4.h"
#include "Connect4/ui/cursorManager.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/constants.h"
#include "Connect4/ui/screens/menu.h"
#include "Connect4/ui/screens/game.h"
#include "Connect4/ui/screens/settings.h"
#include "Connect4/assets/sounds.h"
#include <stdlib.h>
#include <time.h>

bool Connect4_Init_Dependencies() {
    srand(time(NULL));
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        // The error is inserted at %s
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }
    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        return false;
    }
    if (!C4_InitAudio()) {
        SDL_Log("C4_InitAudio failed");
        return false;
    }
    return true;
}

void Connect4_Quit_Dependencies() {
    TTF_Quit();
    SDL_Quit();
    C4_QuitAudio();
    C4_DestroyAllCursors();
    C4_CloseAllFonts();
}

// Returns false if the screen change was unsuccessful (Likely out of memory)
static bool C4_Game_HandleScreenChangeRequest(C4_Game* game, C4_ScreenChangeRequest type) {
    if (type != C4_ScreenChangeRequest_None && game->currentScreen.Destroy) {
        game->currentScreen.Destroy(game->currentScreen.data);
    }
    switch (type) {
        case C4_ScreenChangeRequest_Menu: {
            game->currentScreen.data = C4_MenuScreen_Create(game->renderer);
            game->currentScreen.Destroy = &C4_MenuScreen_Destroy;
            game->currentScreen.Draw = &C4_MenuScreen_Draw;
            game->currentScreen.HandleKeyboardInput = &C4_MenuScreen_HandleKeyboardInput;
            game->currentScreen.HandleMouseEvents = &C4_MenuScreen_HandleMouseEvents;
        }; break;
        case C4_ScreenChangeRequest_Settings: {
            game->currentScreen.data = C4_SettingsScreen_Create(game->renderer, game->window);
            game->currentScreen.Destroy = &C4_SettingsScreen_Destroy;
            game->currentScreen.Draw = &C4_SettingsScreen_Draw;
            game->currentScreen.HandleKeyboardInput = &C4_SettingsScreen_HandleKeyboardInput;
            game->currentScreen.HandleMouseEvents = &C4_SettingsScreen_HandleMouseEvents;
        }; break;
        case C4_ScreenChangeRequest_Game: {
            game->currentScreen.data = C4_GameScreen_Create(game->renderer, game->board);
            game->currentScreen.Destroy = &C4_GameScreen_Destroy;
            game->currentScreen.Draw = &C4_GameScreen_Draw;
            game->currentScreen.HandleKeyboardInput = &C4_GameScreen_HandleKeyboardInput;
            game->currentScreen.HandleMouseEvents = &C4_GameScreen_HandleMouseEvents;
        }; break;
        case C4_ScreenChangeRequest_CloseWindow: {
            game->running = false;
            return true;
        }; break;
        case C4_ScreenChangeRequest_None: {
            return true;
        }
    }
    if (!game->currentScreen.data) {
        return false;
    }
    SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
    return true;
}

C4_Game* C4_Game_Create(uint8_t boardWidth, uint8_t boardHeight, uint8_t amountToWin) {
    if (amountToWin <= 1) {
        return NULL;
    }
    C4_Game* game = calloc(1, sizeof(C4_Game));
    if (!game) {
        // Out of memory
        return NULL;
    }
    game->window = SDL_CreateWindow("Connect4", 1280, 720, SDL_WINDOW_RESIZABLE);
    if (!game->window) {
        C4_Game_Destroy(game);
        return NULL;
    }
    game->renderer = SDL_CreateRenderer(game->window, NULL);
    if (!game->renderer) {
        C4_Game_Destroy(game);
        return NULL;
    }
    SDL_SetRenderLogicalPresentation(game->renderer, C4_BASE_WINDOW_WIDTH, C4_BASE_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    SDL_SetRenderVSync(game->renderer, 1);
    game->board = C4_Board_Create(boardWidth, boardHeight, amountToWin);
    if (!C4_Game_HandleScreenChangeRequest(game, C4_ScreenChangeRequest_Menu)) {
        C4_Game_Destroy(game);
        return NULL;
    }
    game->running = false;
    game->isFullscreen = false;
    return game;
}

void C4_Game_Destroy(C4_Game* game) {
    if (!game) {
        return;
    }
    if (game->currentScreen.Destroy) {
        game->currentScreen.Destroy(game->currentScreen.data);
    }
    C4_Board_Destroy(game->board);
    if (game->renderer) {
        SDL_DestroyRenderer(game->renderer);
    }
    if (game->window) {
        SDL_DestroyWindow(game->window);
    }
    free(game);
}

static void C4_Game_HandleKeyboardInput(C4_Game* game, int scancode) {
    if (scancode == SDL_SCANCODE_F11) {
        game->isFullscreen = !game->isFullscreen;
        SDL_SetWindowFullscreen(game->window, game->isFullscreen);
        return;
    }
    C4_ScreenChangeRequest request = C4_ScreenChangeRequest_None;
    if (game->currentScreen.HandleKeyboardInput) {
        request = game->currentScreen.HandleKeyboardInput(game->currentScreen.data, scancode);
    }
    if (request != C4_ScreenChangeRequest_None) {
        if (!C4_Game_HandleScreenChangeRequest(game, request)) {
            game->running = false;
        }
    }
}

static void C4_Game_HandleMouseEvents(C4_Game* game, SDL_Event* event) {
    C4_ScreenChangeRequest request = C4_ScreenChangeRequest_None;
    if (game->currentScreen.HandleMouseEvents) {
        request = game->currentScreen.HandleMouseEvents(game->currentScreen.data, event);
    }
    if (request != C4_ScreenChangeRequest_None) {
        if (!C4_Game_HandleScreenChangeRequest(game, request)) {
            game->running = false;
        }
    }
}

static void C4_Game_HandleEvents(C4_Game* game, SDL_Event* event) {
    while (SDL_PollEvent(event)) {
        SDL_ConvertEventToRenderCoordinates(game->renderer, event);
        if (event->type == SDL_EVENT_QUIT) {
            game->running = false;
        } else if (event->type == SDL_EVENT_KEY_DOWN) {
            if (event->key.repeat) {
                continue;
            }
            C4_Game_HandleKeyboardInput(game, event->key.scancode);
        }
        C4_Game_HandleMouseEvents(game, event);
    }
}

void C4_Game_Run(C4_Game* game) {
    game->running = true;
    SDL_Event event;
    while (game->running) {
        C4_Game_HandleEvents(game, &event);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
        SDL_RenderClear(game->renderer);
        if (game->currentScreen.Draw) {
            game->currentScreen.Draw(game->currentScreen.data);
        }
        SDL_RenderPresent(game->renderer);
    }
}