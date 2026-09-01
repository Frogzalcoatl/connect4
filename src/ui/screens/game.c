#include "Connect4/ui/screens/game.h"
#include "Connect4/constants.h"
#include "Connect4/game/events.h"
#include "Connect4/game/consoleOutput.h"
#include "Connect4/ui/element/button.h"
#include "Connect4/assets/fonts.h"
#include <assert.h>

typedef struct {
    C4_Game* game;
    C4_UI_Node* backButton;
} C4_GameScreenData;

static void C4_GameScreen_OnEnter(C4_UI_Screen* screen) {
    assert(screen && screen->data);
    (void)screen;
}

void C4_GameScreen_HandleWindowResize(C4_UI_Screen* screen) {
    assert (screen && screen->data);

    C4_GameScreenData* data = (C4_GameScreenData*)screen->data;

    C4_UI_Canvas_HandleWindowResize(&screen->canvas, data->game->window, data->game->uiScale);
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

static void BackButton(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
}

void C4_GameScreen_Init(C4_UI_Screen* screen, C4_Game* game) {
    assert (screen && game && screen->data && game->renderer);
    
    //C4_UI_Canvas* canvas = &screen->canvas;
    C4_GameScreenData* data = (C4_GameScreenData*)screen->data;
    //SDL_Renderer* renderer = game->renderer;

    data->game = game;

    data->backButton = C4_UI_Button_Create(&screen->canvas.arena, 
        &(C4_UI_Button_Config){
            .style = &C4_UI_THEME_DEFAULT.style,
            .rect = (SDL_FRect){0.f, 0.f, 400.f, 100.f},
            .uiScale = game->uiScale,
            .shapeType = C4_UI_ShapeType_Rectangle,
            .borderWidth = C4_UI_THEME_DEFAULT.borderWidth,
            .text = "Back",
            .font = C4_GetFont(C4_FONT_ASSET_MONOCRAFT, 48.f, TTF_STYLE_BOLD),
            .textEngine = game->textEngine
        }
    );
    data->backButton->input.OnPress = BackButton;
    data->backButton->selfAlign = C4_UI_Align_Bottom;
    C4_UI_Canvas_AddNode(&screen->canvas ,data->backButton);

    screen->HandleWindowResize(screen);
}