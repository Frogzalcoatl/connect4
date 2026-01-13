#include "Connect4/game/connect4.h"
#include "Connect4/ui/cursorStyle.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/constants.h"
#include "Connect4/ui/screens/menu.h"
#include "Connect4/ui/screens/game.h"
#include "Connect4/ui/screens/settings.h"
#include "Connect4/assets/sounds.h"
#include "Connect4/game/events.h"
#include "Connect4/discord-rpc/index.h"
#include "Connect4/android/quit.h"
#include "Connect4/ui/utils.h"
#include "Connect4/input/gamepad.h"
#include "assets_gamecontrollerdb_txt.h"
#include <stdlib.h>
#include <time.h>

void Connect4_ConnectScancodesToInputVerbs(void) {
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_UP, SDL_SCANCODE_UP);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_UP, SDL_SCANCODE_W);

    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_DOWN, SDL_SCANCODE_DOWN);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_DOWN, SDL_SCANCODE_S);

    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_RIGHT, SDL_SCANCODE_RIGHT);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_RIGHT, SDL_SCANCODE_D);

    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_LEFT, SDL_SCANCODE_LEFT);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_LEFT, SDL_SCANCODE_A);

    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_CONFIRM, SDL_SCANCODE_RETURN);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_CONFIRM, SDL_SCANCODE_KP_ENTER);
    // Cancel is set to escape by default dont need to set it here.
}

bool Connect4_Init_Dependencies(void) {

    // To simulate touch events for testing
    // SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        // The error is inserted at %s
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    SDL_IOStream* io = SDL_IOFromConstMem(assets_gamecontrollerdb_txt_data, assets_gamecontrollerdb_txt_size);
    SDL_AddGamepadMappingsFromIO(io, true);
    SDL_AddGamepadMapping(SAHARS_RETROLINK_CONTROLLER_MAPPING);

    // I like logs starting under the file path of the exe in my ide.
    SDL_Log("");
    
    if (!TTF_Init()) {
        SDL_Log("TTF_Init failed: %s", SDL_GetError());
        return false;
    }

    if (!C4_InitAudio()) {
        SDL_Log("C4_InitAudio failed");
        return false;
    }

    C4_Input_Init();
    Connect4_ConnectScancodesToInputVerbs();

    C4_Discord_Init();

    return true;
}

void Connect4_Quit_Dependencies(void) {
    C4_Discord_Shutdown();
    C4_Input_Shutdown();
    C4_CloseAllFonts();
    C4_DestroyAllCursors();
    C4_QuitAudio();
    TTF_Quit();
    SDL_Quit();
}

static void C4_Game_UpdateWindowProperties(C4_Game* game, unsigned int actualWindowWidth, unsigned int actualWindowHeight) {
    game->currentLayout = C4_UI_GetCurrentLayout(actualWindowWidth, actualWindowHeight);
    if (game->currentLayout == C4_UI_LayoutType_Wide) {
        game->presentationWidth = C4_WIDE_LAYOUT_BASE_WINDOW_WIDTH;
        game->presentationHeight = C4_WIDE_LAYOUT_BASE_WINDOW_HEIGHT;
    } else if (game->currentLayout == C4_UI_LayoutType_Tall) {
        game->presentationWidth = C4_TALL_LAYOUT_BASE_WINDOW_WIDTH;
        game->presentationHeight = C4_TALL_LAYOUT_BASE_WINDOW_HEIGHT;
    } else {
        game->presentationWidth = 0;
        game->presentationHeight = 0;
    }
    SDL_SetRenderLogicalPresentation(game->renderer, game->presentationWidth, game->presentationHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

static bool C4_Game_WindowSetup(C4_Game* game) {
    SDL_WindowFlags windowFlags;
    #if SDL_PLATFORM_ANDROID || SDL_PLATFORM_IOS
        windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_FULLSCREEN;
        game->isFullscreen = true;
    #else
        windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED;
        game->isFullscreen = false;
    #endif

    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
    // In case the user's monitor size is smaller than 800x600 for whatever reason
    int initialWindowWidth = C4_Min(mode->w, 800);
    int initialWindowHeight = C4_Min(mode->h, 600);

    game->window = SDL_CreateWindow("Connect4", initialWindowWidth, initialWindowHeight, windowFlags);
    if (!game->window) {
        SDL_Log("Unable to create SDL Window: %s", SDL_GetError());
        return false;
    }

    int windowWidth, windowHeight;
    SDL_GetWindowSizeInPixels(game->window, &windowWidth, &windowHeight);
    C4_Game_UpdateWindowProperties(game, windowWidth, windowHeight);

    return true;
}

static bool C4_Game_RendererSetup(C4_Game* game) {
    game->renderer = SDL_CreateRenderer(game->window, NULL);
    if (!game->renderer) {
        SDL_Log("Unable to create SDL renderer: %s", SDL_GetError());
        return false;
    }
    SDL_SetRenderVSync(game->renderer, 1);

    game->textEngine = TTF_CreateRendererTextEngine(game->renderer);
    if (!game->textEngine) {
        SDL_Log("Failed to create text engine: %s", SDL_GetError());
        return false;
    }

    return true;
}

//static void C4_Game_HoverSound(void* context) {
//    (void)context;
//    C4_PlaySound(C4_SoundEffect_ButtonHover);
//}
//
//static void C4_Game_ClickSound(void* context) {
//    (void)context;
//    C4_PlaySound(C4_SoundEffect_ButtonClick);
//}
//
// static void C4_Game_ButtonSounds_SetTouchMode(bool value) {
//     // Touch screens dont need hover sounds
//     // Would be annoying if you swiped your finger across the screen and heard a million button sounds lol
//     if (value) {
//         C4_UI_Button_SetPostCallback(C4_UI_Button_CallbackType_OnHover, NULL, NULL);
//         C4_UI_Button_SetPostCallback(C4_UI_Button_CallbackType_OnRelease, C4_Game_ClickSound, NULL);
//     } else {
//         C4_UI_Button_SetPostCallback(C4_UI_Button_CallbackType_OnHover, C4_Game_HoverSound, NULL);
//         C4_UI_Button_SetPostCallback(C4_UI_Button_CallbackType_OnRelease, C4_Game_ClickSound, NULL);
//     }
// }
// 
// static void C4_Game_TouchModeSetup(void) {
//     #if SDL_PLATFORM_ANDROID || SDL_PLATFORM_IOS
//         C4_Game_ButtonSounds_SetTouchMode(true);
//     #else
//         C4_Game_ButtonSounds_SetTouchMode(false);
//     #endif
//     if (SDL_GetHintBoolean(SDL_HINT_MOUSE_TOUCH_EVENTS, false)) {
//         C4_Game_ButtonSounds_SetTouchMode(true);
//     }
// }

static C4_UI_Screen* (*C4_ScreenCreationArray[C4_ScreenType_ScreenCount])(C4_Game* game) = {
    C4_MenuScreen_Create,
    C4_GameScreen_Create,
    C4_SettingsScreen_Create,
};
static bool C4_Game_CreateScreens(C4_Game* game) {
    for (size_t i = 0; i < C4_ScreenType_ScreenCount; i++) {
        game->screens[i] = C4_ScreenCreationArray[i](game);
        if (!game->screens[i]) {
            SDL_Log("Unable to create screen index: %zu", i);
            return false;
        }
    }
    return true;
}

static void C4_Game_SetScreen(C4_Game* game, C4_ScreenType type) {
    if (!game) {
        SDL_Log("Unable to set screen. C4_Game is NULL");
        return;
    }
    if (type < 0 || type >= C4_ScreenType_ScreenCount) {
        SDL_Log("Unable to set screen. Type is Invalid");
        return;
    }
    if (game->screens[type] == NULL) {
        SDL_Log("Unable to set screen. Screen is NULL");
        return;
    }
    game->currentScreenType = type;
    game->currentScreen = game->screens[type];
    if (game->currentScreen->OnEnter) {
        game->currentScreen->OnEnter(game->currentScreen);
    }
    C4_UI_Canvas_ResetInteractions(&game->currentScreen->canvas);
    SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
    if (game->currentScreen->HandleWindowResize) {
        game->currentScreen->HandleWindowResize(game->currentScreen, game->currentLayout);
    }

    if (game->currentScreenType == C4_ScreenType_Menu) {
        C4_Discord_UpdateStatus("In the Menus", NULL);
    } else if (game->currentScreenType == C4_ScreenType_Game) {
        C4_Discord_UpdateStatus("In Multiplayer", NULL);
    }
}

C4_Game* C4_Game_Create(uint8_t boardWidth, uint8_t boardHeight, uint8_t amountToWin) {
    C4_Game* game = calloc(1, sizeof(C4_Game));
    if (!game) {
        SDL_Log("Unable allocate memory for C4 Game");
        return NULL;
    }

    if (!C4_Game_WindowSetup(game)) {
        C4_Game_Destroy(game);
        return NULL;
    }

    if (!C4_Game_RendererSetup(game)) {
        C4_Game_Destroy(game);
        return NULL;
    }

    game->board = C4_Board_Create(boardWidth, boardHeight, amountToWin);

    // C4_Game_TouchModeSetup();

    game->UIScale = 1.f;
    game->running = false;

    if (!C4_Game_CreateScreens(game)) {
        C4_Game_Destroy(game);
        return NULL;
    }

    C4_Game_SetScreen(game, C4_ScreenType_Menu);

    return game;
}

void C4_Game_Destroy(C4_Game* game) {
    if (!game) {
        SDL_Log("Tried to destroy NULL C4 Game");
        return;
    }
    for (size_t i = 0; i < C4_ScreenType_ScreenCount; i++) {
        if (game->screens[i]->Destroy) {
            game->screens[i]->Destroy(game->screens[i]);
        }
        game->screens[i] = NULL;
    }
    game->currentScreen = NULL;
    C4_Board_Destroy(game->board);
    if (game->textEngine) {
        TTF_DestroyRendererTextEngine(game->textEngine);
    }
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
}

static void C4_Game_HandleEvents(C4_Game* game, SDL_Event* eventSDL, C4_Event* eventC4) {
    while (SDL_PollEvent(eventSDL)) {
        if (eventSDL->type == SDL_EVENT_QUIT) {
            game->running = false;
        } else if (eventSDL->type == SDL_EVENT_KEY_DOWN) {
            if (!eventSDL->key.repeat) {
                C4_Game_HandleKeyboardInput(game, eventSDL->key.scancode);
            }
        } else if (eventSDL->type == SDL_EVENT_WINDOW_RESIZED) {
            unsigned int windowWidth = eventSDL->window.data1;
            unsigned int windowHeight = eventSDL->window.data2;
            C4_Game_UpdateWindowProperties(game, windowWidth, windowHeight);
            game->currentScreen->HandleWindowResize(game->currentScreen, game->currentLayout);
        }
        SDL_ConvertEventToRenderCoordinates(game->renderer, eventSDL);
        if (game->currentScreen->HandleEvent) {
            game->currentScreen->HandleEvent(game->currentScreen, eventSDL, game->UIScale);
        }
    }
    while (C4_PollEvent(eventC4)) {
        switch (eventC4->type) {
            case C4_EVENT_CLOSE_WINDOW: {
                game->running = false;
                #ifdef SDL_PLATFORM_ANDROID
                    Android_QuitTask(eventC4->closeWindow.androidRemoveTask);
                #endif
            }; break;
            case C4_EVENT_SCREEN_CHANGE: {
                C4_Game_SetScreen(game, eventC4->screenChange.type);
            }; break;
            default: break;
        }
    }
}

void C4_Game_Run(C4_Game* game) {
    game->running = true;
    SDL_Event eventSDL;
    C4_Event eventC4;
    Uint64 lastTicks = SDL_GetTicks();
    while (game->running) {
        Uint64 currentTicks = SDL_GetTicks();
        float deltaTime = (float)(currentTicks - lastTicks) / 1000.0f;
        lastTicks = currentTicks;

        C4_Game_HandleEvents(game, &eventSDL, &eventC4);
        if (game->currentScreen->Update) {
            game->currentScreen->Update(game->currentScreen, deltaTime);
        }

        SDL_SetRenderDrawColor(game->renderer, C4_WINDOW_BG_COLOR.r, C4_WINDOW_BG_COLOR.g, C4_WINDOW_BG_COLOR.b, C4_WINDOW_BG_COLOR.a);
        SDL_RenderClear(game->renderer);

        if (game->currentScreen->Draw) {
            game->currentScreen->Draw(game->currentScreen, game->UIScale);
        }

        SDL_RenderPresent(game->renderer);

        C4_Discord_Loop();
    }
}