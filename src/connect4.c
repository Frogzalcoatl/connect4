#include "Connect4/connect4.h"
#include "Connect4/ui/cursorManager.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/constants.h"
#include "Connect4/game/screens/index.h"
#include "Connect4/game/screens/menu.h"
#include "Connect4/game/screens/game.h"
#include "Connect4/game/screens/settings.h"
#include "Connect4/assets/sounds.h"
#include "Connect4/game/events.h"
#include "Connect4/discord-rpc/index.h"
#include "Connect4/android/quit.h"
#include <stdlib.h>
#include <time.h>

bool Connect4_Init_Dependencies(void) {

    // To simulate touch events for testing
    // SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");

    // Force Android to use the older, often more stable OpenSL ES driver
    // The app was making loud ah clicking sounds when i tested it on my phone without this
    #if SDL_PLATFORM_ANDROID
        if (!SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "openslES")) {
            SDL_ResetHint(SDL_HINT_AUDIO_DRIVER);
        }
    #endif

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        // The error is inserted at %s
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

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

    C4_Discord_Init();

    return true;
}

void Connect4_Quit_Dependencies(void) {
    C4_Discord_Shutdown();
    C4_CloseAllFonts();
    C4_DestroyAllCursors();
    C4_QuitAudio();
    TTF_Quit();
    SDL_Quit();
}

static void C4_Game_UpdateWindowProperties(C4_Game* game) {
    if (game->currentLayout == C4_UI_LayoutType_Wide) {
        game->windowWidth = C4_WIDE_LAYOUT_BASE_WINDOW_WIDTH;
        game->windowHeight = C4_WIDE_LAYOUT_BASE_WINDOW_HEIGHT;
    } else if (game->currentLayout == C4_UI_LayoutType_Tall) {
        game->windowWidth = C4_TALL_LAYOUT_BASE_WINDOW_WIDTH;
        game->windowHeight = C4_TALL_LAYOUT_BASE_WINDOW_HEIGHT;
    } else {
        game->windowWidth = 0;
        game->windowHeight = 0;
    }
    game->currentLayout = C4_UI_GetCurrentLayout(game->windowWidth, game->windowHeight);
    SDL_SetRenderLogicalPresentation(game->renderer, game->windowWidth, game->windowHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

static void C4_Game_ChangeScreen(C4_Game* game, C4_ScreenType type) {
    if (!game) {
        SDL_Log("Unable to change game screen. Game is NULL");
        return;
    }
    if (type < 0 || type >= C4_ScreenType_ScreenCount) {
        SDL_Log("Unable to change game screen. Type is Invalid");
        return;
    }
    if (type == game->currentScreen) {
        return;
    }
    C4_Game_UpdateWindowProperties(game);
    switch (type) {
        case C4_ScreenType_Menu: {
            C4_SetScreen_Menu(game);
            C4_Discord_UpdateStatus("In the Menus", NULL);
        }; break;
        case C4_ScreenType_Settings: {
            C4_SetScreen_Settings(game);
        }; break;
        case C4_ScreenType_Game: {
            C4_SetScreen_Game(game);
            C4_Discord_UpdateStatus("In 2 Player Mode", NULL);
        }; break;
        default: break;
    }
    SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
    return;
}

static void C4_Game_HoverSound(void* context) {
    (void)context;
    C4_PlaySound(C4_SoundEffect_ButtonHover);
}

static void C4_Game_ClickSound(void* context) {
    (void)context;
    C4_PlaySound(C4_SoundEffect_ButtonClick);
}

static void C4_Game_ButtonSounds_SetTouchMode(bool value) {
    // Touch screens hear the hover sound when they initially press the button
    if (value) {
        C4_UI_Button_SetPostCallback(C4_UI_Button_CallbackType_OnPress, C4_Game_HoverSound, NULL);
        C4_UI_Button_SetPostCallback(C4_UI_Button_CallbackType_OnHover, NULL, NULL);
    } else {
        C4_UI_Button_SetPostCallback(C4_UI_Button_CallbackType_OnPress, NULL, NULL);
        C4_UI_Button_SetPostCallback(C4_UI_Button_CallbackType_OnHover, C4_Game_HoverSound, NULL);
    }
}

C4_Game* C4_Game_Create(uint8_t boardWidth, uint8_t boardHeight, uint8_t amountToWin) {
    C4_Game* game = calloc(1, sizeof(C4_Game));
    if (!game) {
        SDL_Log("Unable allocate memory for C4 Game");
        return NULL;
    }

    int initialWindowWidth = 1280;
    int initialWindowHeight = 720;

    game->window = SDL_CreateWindow("Connect4", initialWindowWidth, initialWindowHeight, SDL_WINDOW_RESIZABLE);
    if (!game->window) {
        SDL_Log("Unable to create SDL Window: %s", SDL_GetError());
        C4_Game_Destroy(game);
        return NULL;
    }
    
    game->renderer = SDL_CreateRenderer(game->window, NULL);
    if (!game->renderer) {
        SDL_Log("Unable to create SDL renderer: %s", SDL_GetError());
        C4_Game_Destroy(game);
        return NULL;
    }
    SDL_SetRenderVSync(game->renderer, 1);

    SDL_GetWindowSizeInPixels(game->window, &initialWindowWidth, &initialWindowHeight);
    game->currentLayout = C4_UI_GetCurrentLayout(initialWindowWidth, initialWindowHeight);
    C4_Game_UpdateWindowProperties(game);

    game->board = C4_Board_Create(boardWidth, boardHeight, amountToWin);

    game->fontRegular = C4_GetFont(C4_FontType_Regular);
    game->fontBold = C4_GetFont(C4_FontType_Bold);

    C4_UI_Button_SetPostCallback(C4_UI_Button_CallbackType_OnRelease, C4_Game_ClickSound, NULL);

    #if SDL_PLATFORM_ANDROID || SDL_PLATFORM_IOS
        C4_Game_ButtonSounds_SetTouchMode(true);
    #else
        C4_Game_ButtonSounds_SetTouchMode(false);
    #endif
    if (SDL_GetHintBoolean(SDL_HINT_MOUSE_TOUCH_EVENTS, false)) {
        C4_Game_ButtonSounds_SetTouchMode(true);
    }

    C4_UI_Container_Init(&game->container, game->renderer, 0.f, 0.f);

    game->UIScale = 1.f;
    C4_Game_ChangeScreen(game, C4_ScreenType_Menu);
    game->running = false;
    game->isFullscreen = false;
    return game;
}

void C4_Game_Destroy(C4_Game* game) {
    if (!game) {
        SDL_Log("Tried to destroy NULL C4 Game");
        return;
    }
    C4_UI_Container_Destroy(&game->container);
    C4_Board_Destroy(game->board);
    if (game->renderer) {
        SDL_DestroyRenderer(game->renderer);
    }
    if (game->window) {
        SDL_DestroyWindow(game->window);
    }
    SDL_free(game);
}

static void C4_Game_HandleKeyboardInput(C4_Game* game, int scancode) {
    if (scancode == SDL_SCANCODE_F11) {
        game->isFullscreen = !game->isFullscreen;
        SDL_SetWindowFullscreen(game->window, game->isFullscreen);
        return;
    }
}

static void C4_Game_UpdateUI(C4_Game* game) {
    C4_Game_UpdateWindowProperties(game);
    switch (game->currentScreen) {
        case C4_ScreenType_Menu: {
            C4_UpdateUILayout_Menu(game->currentLayout);
        }; break;
        case C4_ScreenType_Settings: {
            C4_UpdateUILayout_Settings(game->currentLayout);
        }; break;
        case C4_ScreenType_Game: {
            C4_UpdateUILayout_Game(game->currentLayout);
        }; break;
        default: break;
    }
}

static void C4_Game_HandleEvents(C4_Game* game, SDL_Event* eventSDL, C4_Event* eventC4) {
    while (SDL_PollEvent(eventSDL)) {
        SDL_ConvertEventToRenderCoordinates(game->renderer, eventSDL);
        if (eventSDL->type == SDL_EVENT_QUIT) {
            game->running = false;
        } else if (eventSDL->type == SDL_EVENT_KEY_DOWN) {
            if (eventSDL->key.repeat) {
                continue;
            }
            C4_Game_HandleKeyboardInput(game, eventSDL->key.scancode);
        } else if (eventSDL->type == SDL_EVENT_WINDOW_RESIZED) {
            game->currentLayout= C4_UI_GetCurrentLayout(eventSDL->window.data1, eventSDL->window.data2);
            C4_Game_UpdateUI(game);
        }
        C4_UI_Container_HandleEvent(&game->container, eventSDL, game->UIScale);
    }
    while (C4_PollEvent(eventC4)) {
        switch (eventC4->type) {
            case C4_EVENT_CLOSE_WINDOW: {
                game->running = false;
                #ifdef SDL_PLATFORM_ANDROID
                    Android_QuitAndRemoveTask();
                #endif
            }; break;
            case C4_EVENT_SCREEN_CHANGE: {
                C4_Game_ChangeScreen(game, eventC4->screenChange.type);
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
        C4_UI_Container_Update(&game->container, deltaTime);

        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
        SDL_RenderClear(game->renderer);

        C4_UI_Container_Draw(&game->container, game->UIScale);

        SDL_RenderPresent(game->renderer);

        C4_Discord_Loop();
    }
}