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
#include "Connect4/tools/virtualFileSystem.h"
#include "Connect4/dat.h"
#include "Connect4/game/consoleOutput.h"
#include <stdlib.h>
#include <time.h>
#include <assert.h>

void Connect4_ConnectScancodesToInputVerbs(void) {
    // Arrow keys, enter, and esc are already setup by default
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_UP, SDL_SCANCODE_W);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_DOWN, SDL_SCANCODE_S);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_RIGHT, SDL_SCANCODE_D);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_LEFT, SDL_SCANCODE_A);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_CONFIRM, SDL_SCANCODE_SPACE);
}

static bool C4_AddMappingFromVFS(const char* filePath) {
    assert(filePath);
    
    size_t len;
    void* rawData = C4_VFS_ReadFile(filePath, &len);
    if (!rawData) {
        return false;
    }
    SDL_IOStream* io = SDL_IOFromConstMem(rawData, len);
    if (!io) {
        return false;
    }
    const int mappingsAdded = SDL_AddGamepadMappingsFromIO(io, true);
    C4_VFS_FreeFile(rawData);
    C4_Log("Added %d gamepad mappings from file %s", mappingsAdded, filePath);
    return true;
}

void Connect4_Init_Dependencies(void) {

    // To simulate touch events for testing
    // SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        C4_FatalError(C4_ErrorCode_DependencyErrorSDL, "%s", SDL_GetError());
    }
    // I like logs starting under the file path of the exe in my ide.
    SDL_Log(" ");
    C4_Log("Initialized SDL with flags: SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD");

    const char* basePath = SDL_GetBasePath();

    char fullDatPath[2048];
    if (basePath) {
        SDL_snprintf(fullDatPath, sizeof(fullDatPath), "%s%s", basePath, MASTER_DAT_PATH);
    } else {
        SDL_strlcpy(fullDatPath, MASTER_DAT_PATH, sizeof(fullDatPath));
    }

    C4_VFS_Init(fullDatPath);
    
    if (!TTF_Init()) {
        C4_FatalError(C4_ErrorCode_DependencyErrorTTF, "%s", SDL_GetError());
    }
    C4_Log("Initialized SDL_ttf");

    if (!MIX_Init()) {
        C4_FatalError(C4_ErrorCode_DependencyErrorMIX, "%s", SDL_GetError());
    }
    C4_Log("Initialized SDL_mixer");

    C4_Input_Init();

    const char controllerDbPath[] = "assets/gamecontrollerdb.txt";
    if (!C4_AddMappingFromVFS(controllerDbPath)) {
        C4_Warn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Unable to add gamepad mappings from file: %s",
            controllerDbPath
        );
    }

    SDL_AddGamepadMapping(SAHARS_RETROLINK_CONTROLLER_MAPPING_WINDOWS);
    SDL_AddGamepadMapping(SAHARS_RETROLINK_CONTROLLER_MAPPING_LINUX);
    C4_Log("Added gamepad mapping \"SAHARS_RETROLINK_CONTROLLER\"");

    Connect4_ConnectScancodesToInputVerbs();

    C4_Discord_Init();
}

void Connect4_Quit_Dependencies(void) {
    C4_Discord_Shutdown();

    C4_Input_Shutdown();

    C4_CloseAllFonts();

    C4_DestroyAllCursors();

    C4_VFS_Quit();

    MIX_Quit();
    C4_Log("Quit SDL_mixer");

    TTF_Quit();
    C4_Log("Quit SDL_ttf");

    SDL_Quit();
    C4_Log("Quit SDL");
}

static void C4_Game_RecalculateScale(C4_Game* game) {
    assert(game);

    const float REFERENCE_HEIGHT = 1080.0f;
    const float REFERENCE_WIDTH = 1920.f;
    float scaleW = (float)game->windowWidth / REFERENCE_WIDTH;
    float scaleH = (float)game->windowHeight / REFERENCE_HEIGHT;

    if (game->windowWidth < game->windowHeight) {
        game->UIScale = scaleW;
    } else {
        game->UIScale = scaleH;
    }

    game->UIScale *= game->userScalePreference;

    if (game->UIScale < 0.1f) {
        game->UIScale = 0.1f;
    }
}
/*
static void C4_Game_SetUserScale(C4_Game* game, float newScale) {
    assert(game);

    game->userScalePreference = newScale;
    
    C4_Game_RecalculateScale(game);

    if (game->currentScreen && game->currentScreen->HandleWindowResize) {
        game->currentScreen->HandleWindowResize(game->currentScreen, game->currentLayout);
    }
}
*/
SDL_FPoint C4_GetReferenceWindowDimensions(unsigned int w, unsigned int h, float UIScale) {
    float width = w / UIScale;
    float height = h / UIScale;
    return (SDL_FPoint){width, height};
}

static void C4_Game_UpdateWindowProperties(C4_Game* game, unsigned int windowWidth, unsigned int windowHeight) {
    assert(game);

    game->currentLayout = C4_UI_GetCurrentLayout(windowWidth, windowHeight);
    game->windowWidth = windowWidth;
    game->windowHeight = windowHeight;
    
    C4_Game_RecalculateScale(game);
}

static void C4_Game_WindowSetup(C4_Game* game) {
    assert(game);

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
    int initialWindowWidth = SDL_min(mode->w, 800);
    int initialWindowHeight = SDL_min(mode->h, 600);

    game->window = SDL_CreateWindow("Connect4", initialWindowWidth, initialWindowHeight, windowFlags);
    if (!game->window) {
        C4_FatalError(C4_ErrorCode_RipConnect4Struct, "Unable to set up SDL window");
    }

    int windowWidth, windowHeight;
    SDL_GetWindowSizeInPixels(game->window, &windowWidth, &windowHeight);
    C4_Game_UpdateWindowProperties(game, windowWidth, windowHeight);

    C4_Log("SDL window setup complete");
}

static C4_UI_Screen* (*C4_ScreenCreationArray[C4_ScreenType_ScreenCount])(C4_Game* game) = {
    C4_MenuScreen_Create,
    C4_GameScreen_Create,
    C4_SettingsScreen_Create,
};
static void C4_Game_CreateScreens(C4_Game* game) {
    assert(game);

    for (size_t i = 0; i < C4_ScreenType_ScreenCount; i++) {
        game->screens[i] = C4_ScreenCreationArray[i](game);
        if (!game->screens[i]) {
            C4_FatalError(
                C4_ErrorCode_RipConnect4Struct,
                "Unable to create screen index: %zu",
                i
            );
        }
    }

    C4_Log("C4 screen creation complete");
}

static void C4_Game_SetScreen(C4_Game* game, C4_ScreenType type) {
    assert(game);
    assert(type >= 0 && type < C4_ScreenType_ScreenCount);
    assert(game->screens[type]);

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
    C4_Game* game = SDL_calloc(1, sizeof(C4_Game));
    if (!game) {
        C4_FatalError(C4_ErrorCode_OutOfMemory, "Unable allocate memory for C4 Game");
    }

    game->userScalePreference = 1.f;

    C4_Game_WindowSetup(game);

    game->renderer = SDL_CreateRenderer(game->window, NULL);
    if (!game->renderer) {
        C4_FatalError(
            C4_ErrorCode_RipConnect4Struct,
            "Unable to create SDL renderer: %s",
            SDL_GetError()
        );
    }
    SDL_SetRenderVSync(game->renderer, 1);
    C4_Log("SDL renderer setup complete");

    game->textEngine = TTF_CreateRendererTextEngine(game->renderer);
    if (!game->textEngine) {
        C4_FatalError(
            C4_ErrorCode_RipConnect4Struct,
            "Unable to create SDL text engine: %s",
            SDL_GetError()
        );
    }
    C4_Log("SDL_ttf text engine setup complete");

    // Game can still function if the soundsystem is null. Just no audio rip
    game->soundSystem = C4_SoundSystem_Create();
    if (!game->soundSystem) {
        const char audioWarning[] = "Unable to initialize sound system. Application will continue without audio.";
        C4_Warn(SDL_LOG_CATEGORY_AUDIO, audioWarning);
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_WARNING,
            "Connect4 Warning",
            audioWarning,
            game->window
        );
    }
    C4_SoundSystem_SetVolume(game->soundSystem, C4_AudioTrack_Music, 0.3f);
    C4_SoundSystem_PlayMusic(game->soundSystem, C4_MusicTrack_Test);

    game->board = C4_Board_Create(boardWidth, boardHeight, amountToWin);

    game->running = false;

    C4_Game_CreateScreens(game);

    C4_Game_SetScreen(game, C4_ScreenType_Menu);

    return game;
}

void C4_Game_Destroy(C4_Game* game) {
    assert(game);

    for (size_t i = 0; i < C4_ScreenType_ScreenCount; i++) {
        if (game->screens[i]->Destroy) {
            game->screens[i]->Destroy(game->screens[i]);
        }
        game->screens[i] = NULL;
    }
    game->currentScreen = NULL;
    C4_Board_Destroy(game->board);
    if (game->soundSystem) {
        C4_SoundSystem_Destroy(game->soundSystem);
    }
    if (game->textEngine) {
        TTF_DestroyRendererTextEngine(game->textEngine);
    }
    if (game->renderer) {
        SDL_DestroyRenderer(game->renderer);
    }
    if (game->window) {
        SDL_DestroyWindow(game->window);
    }
    SDL_free(game);
}

static void C4_Game_HandleKeyboardInput(C4_Game* game, int scancode) {
    assert(game);

    if (scancode == SDL_SCANCODE_F11) {
        game->isFullscreen = !game->isFullscreen;
        SDL_SetWindowFullscreen(game->window, game->isFullscreen);
        return;
    }
}

static void C4_Game_HandleEvents(C4_Game* game, SDL_Event* eventSDL, C4_Event* eventC4) {
    assert(game);
    assert(eventSDL);
    assert(eventC4);
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
            if (game->currentScreen->HandleWindowResize) {
                game->currentScreen->HandleWindowResize(game->currentScreen, game->currentLayout);
            }
        }
        SDL_ConvertEventToRenderCoordinates(game->renderer, eventSDL);
        if (game->currentScreen->HandleEvent) {
            game->currentScreen->HandleEvent(game->currentScreen, game->window, eventSDL);
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
            case C4_EVENT_SET_CURSOR: {
                SDL_SetCursor(C4_GetSystemCursor(eventC4->setCursor.type));
            }; break;
            case C4_EVENT_PLAY_SOUND: {
                C4_SoundSystem_PlaySound(game->soundSystem, eventC4->playSound.id);
            }; break;
            case C4_EVENT_PLAY_MUSIC: {
                C4_SoundSystem_PlayMusic(game->soundSystem, eventC4->playMusic.id);
            }; break;
            case C4_EVENT_SET_VOLUME: {
                C4_SoundSystem_SetVolume(game->soundSystem, eventC4->setVolume.track, eventC4->setVolume.level);
            }; break;
            default: break;
        }
    }
}

void C4_Game_Run(C4_Game* game) {
    assert(game);
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
            game->currentScreen->Update(game->currentScreen, deltaTime, game->UIScale);
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