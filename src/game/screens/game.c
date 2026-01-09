#include "Connect4/game/screens/game.h"
#include "Connect4/constants.h"
#include "Connect4/game/events.h"

typedef struct {
    C4_Game* game;
    C4_UI_Text* boardText;
    C4_UI_Popup* winnerPopup;
    C4_UI_Button* backButton;
} C4_GameScreenData;

static void C4_GameScreen_OnEnter(C4_UI_Screen* screen) {
    if (!screen || !screen->data) {
        return;
    }
    C4_GameScreenData* data = (C4_GameScreenData*)screen->data;
    data->winnerPopup->isShowing = false;
    C4_Board_UpdateTestStr(data->game->board, data->boardText->str, sizeof(data->boardText->str));
    C4_UI_Text_ReloadTexture(data->boardText, data->game->renderer);
    C4_UI_CenterInWindow(&data->boardText->destination, C4_Axis_X, data->game->presentationWidth, data->game->presentationHeight, data->game->UIScale);
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
            gameData->boardText->destination.y = 0.f;
            C4_UI_CenterInWindow(&gameData->boardText->destination, C4_Axis_X, game->presentationWidth, game->presentationHeight, game->UIScale);
        }; break;
        case C4_UI_LayoutType_Tall: {
            C4_UI_CenterInWindow(&gameData->boardText->destination, C4_Axis_XY, game->presentationWidth, game->presentationHeight, game->UIScale);
        }; break;
        default: break;
    }
    C4_UI_Popup_CenterInWindow(gameData->winnerPopup, renderer, game->presentationWidth, game->presentationHeight, game->UIScale);
    SDL_FRect* backButtonDest = &gameData->backButton->background.destination;
    backButtonDest->y = game->presentationHeight - backButtonDest->h - 50.f;
    C4_UI_Button_ChangeDestination(gameData->backButton, gameData->backButton->background.destination);
    C4_UI_Button_CenterInWindow(gameData->backButton, C4_Axis_X, game->presentationWidth, game->presentationHeight, game->UIScale);
}

static void C4_GameScreen_ResetGame(C4_GameScreenData* gameData) {
    C4_Board_Reset(gameData->game->board);
    C4_Board_UpdateTestStr(gameData->game->board, gameData->boardText->str, sizeof(gameData->boardText->str));
    C4_UI_Text_ReloadTexture(gameData->boardText, gameData->game->renderer);
}

static void PlayAgainButton_OnRelease(void* context) {
    if (!context) {
        return;
    }
    C4_GameScreenData* gameData = (C4_GameScreenData*)context;
    gameData->winnerPopup->isShowing = false;
    C4_GameScreen_ResetGame(gameData);
}
static void ReturnToMenuButton_OnRelease(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
}

static void BackButton_OnRelease(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
}

static bool C4_GameScreen_Init(C4_UI_Screen* screen, C4_Game* game);

C4_UI_Screen* C4_GameScreen_Create(C4_Game* game) {
    if (!game) {
        return NULL;
    }
    C4_UI_Screen* screen = C4_Screen_Create(game->renderer);
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

    data->boardText = C4_UI_Canvas_Add_Text(
        canvas, &(C4_UI_Text_Config){
            .str = "",
            .font = game->fontRegular,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = 140.f,
            .destinationX = 0.f,
            .destinationY = 0.f,
            .wrapWidth = 0
        }
    );
    if (!data->boardText) {
        return false;
    }
    C4_Board_UpdateTestStr(game->board, data->boardText->str, sizeof(data->boardText->str));
    C4_UI_Text_ReloadTexture(data->boardText, renderer);

    data->backButton = C4_UI_Canvas_Add_Button(
        canvas, &(C4_UI_Button_Config){
            .text = "Back",
            .destination = (SDL_FRect){0.f, 0.f, 400.f, 100.f},
            .symbol = C4_UI_SymbolType_None,
            .font = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->backButton) {
        return false;
    }
    data->backButton->OnReleaseCallback = BackButton_OnRelease;

    data->winnerPopup = C4_UI_Canvas_Add_Popup(
        canvas, &(C4_UI_Popup_Config){
            .destination = (SDL_FRect){0.f, 0.f, 800.f, 400.f},
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .buttonCount = 2,
            .buttonGroupHeight = 100.f,
            .message = "",
            .messageFont = game->fontRegular,
            .buttonFont = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->winnerPopup) {
        return false;
    }

    C4_UI_Button* playAgainButton = &data->winnerPopup->buttonGroup.buttons[0];
    C4_UI_Text_UpdateStr(&playAgainButton->text, "Play Again", renderer);
    playAgainButton->OnReleaseCallback = PlayAgainButton_OnRelease;
    playAgainButton->OnReleaseContext = data;

    C4_UI_Button* returnToMenuButton = &data->winnerPopup->buttonGroup.buttons[1];
    C4_UI_Text_UpdateStr(&returnToMenuButton->text, "Return to Menu", renderer);
    returnToMenuButton->OnReleaseCallback = ReturnToMenuButton_OnRelease;

    screen->HandleWindowResize(screen, game->currentLayout);

    return true;
}