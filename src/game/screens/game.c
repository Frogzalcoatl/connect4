#include "Connect4/game/screens/game.h"
#include "Connect4/constants.h"
#include "Connect4/game/events.h"

typedef struct {
    C4_Game* game;
    C4_UI_Text* boardText;
    C4_UI_Popup* winnerPopup;
    C4_UI_Button* backButton;
} C4_GameScreenData;
static C4_GameScreenData gameData;

void C4_UpdateUILayout_Game(C4_UI_LayoutType layout) {
    C4_Game* game = gameData.game;
    SDL_Renderer* renderer = game->renderer;

    switch (layout) {
        case C4_UI_LayoutType_Wide: {
            gameData.boardText->destination.y = 0.f;
            C4_UI_CenterInWindow(&gameData.boardText->destination, C4_Axis_X, game->windowWidth, game->windowHeight, game->UIScale);
        }; break;
        case C4_UI_LayoutType_Tall: {
            C4_UI_CenterInWindow(&gameData.boardText->destination, C4_Axis_XY, game->windowWidth, game->windowHeight, game->UIScale);
        }; break;
        default: break;
    }
    C4_UI_Popup_CenterInWindow(gameData.winnerPopup, renderer, game->windowWidth, game->windowHeight, game->UIScale);
    SDL_FRect* backButtonDest = &gameData.backButton->background.destination;
    backButtonDest->y = game->windowHeight - backButtonDest->h - 50.f;
    C4_UI_Button_ChangeDestination(gameData.backButton, gameData.backButton->background.destination);
    C4_UI_Button_CenterInWindow(gameData.backButton, C4_Axis_X, game->windowWidth, game->windowHeight, game->UIScale);
}

static void C4_GameScreen_ResetGame() {
    C4_Board_Reset(gameData.game->board);
    C4_Board_UpdateTestStr(gameData.game->board, gameData.boardText->str, sizeof(gameData.boardText->str));
    C4_UI_Text_ReloadTexture(gameData.boardText, gameData.game->renderer);
}

static void PlayAgainOnClick(void* context) {
    (void)context;
    gameData.winnerPopup->isShowing = false;
    C4_GameScreen_ResetGame();
}
static void ReturnToMenuOnClick(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
}

static void BackOnClick(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
}

void C4_SetScreen_Game(C4_Game* game) {
    if (!game) {
        return;
    }
    C4_UI_Container* cont = &game->container;
    C4_UI_Container_Clear(cont);
    game->currentScreen = C4_ScreenType_Game;
    gameData.game = game;

    gameData.boardText = C4_UI_Container_Add_Text(
        cont, &(C4_UI_Text_Config){
            .str = "",
            .font = game->fontRegular,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = 140.f,
            .destinationX = 0.f,
            .destinationY = 0.f,
            .wrapWidth = 0
        }
    );
    C4_Board_UpdateTestStr(game->board, gameData.boardText->str, sizeof(gameData.boardText->str));
    C4_UI_Text_ReloadTexture(gameData.boardText, game->renderer);

    gameData.backButton = C4_UI_Container_Add_Button(
        cont, &(C4_UI_Button_Config){
            .text = "Back",
            .destination = (SDL_FRect){0.f, 0.f, 400.f, 100.f},
            .symbol = C4_UI_SymbolType_None,
            .font = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    gameData.backButton->OnReleaseCallback = BackOnClick;

    gameData.winnerPopup = C4_UI_Container_Add_Popup(
        cont, &(C4_UI_Popup_Config){
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

    C4_UI_Button* playAgainButton = &gameData.winnerPopup->buttonGroup.buttons[0];
    C4_UI_Text_UpdateStr(&playAgainButton->text, "Play Again", game->renderer);
    playAgainButton->OnReleaseCallback = PlayAgainOnClick;

    C4_UI_Button* returnToMenuButton = &gameData.winnerPopup->buttonGroup.buttons[1];
    C4_UI_Text_UpdateStr(&returnToMenuButton->text, "Return to Menu", game->renderer);
    returnToMenuButton->OnReleaseCallback = ReturnToMenuOnClick;

    C4_UpdateUILayout_Game(game->currentLayout);
}