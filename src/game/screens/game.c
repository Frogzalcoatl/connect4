#include "Connect4/game/screens/game.h"
#include "Connect4/constants.h"
#include "Connect4/game/events.h"

typedef struct {
    C4_Game* game;
    C4_UI_Text* boardText;
    C4_UI_Popup* winnerPopup;
} C4_GameScreenData;
static C4_GameScreenData gameData;

static void C4_GameScreen_ResetGame() {
    C4_Board_Reset(gameData.game->board);
    C4_Board_UpdateTestStr(gameData.game->board, gameData.boardText->str, sizeof(gameData.boardText->str));
    C4_UI_Text_ReloadTexture(gameData.boardText, gameData.game->renderer);
}

static void PlayAgainOnClick(void* context) {
    (void)context;
    gameData.winnerPopup->isShowing = false;
    
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

    C4_UI_Text* boardText = C4_UI_Container_Add_Text(
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
    C4_Board_UpdateTestStr(game->board, boardText->str, sizeof(boardText->str));
    C4_UI_Text_ReloadTexture(boardText, game->renderer);
    C4_UI_CenterInWindow(&boardText->destination, C4_Axis_X, C4_BASE_WINDOW_WIDTH, C4_BASE_WINDOW_HEIGHT);

    C4_UI_Button* backButton = C4_UI_Container_Add_Button(
        cont, &(C4_UI_Button_Config){
            .text = "Back",
            .destination = (SDL_FRect){0.f, 950.f, 400.f, 100.f},
            .symbol = C4_UI_SymbolType_None,
            .font = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    backButton->OnClickCallback = BackOnClick;
    C4_UI_Button_CenterInWindow(backButton, C4_Axis_X, C4_BASE_WINDOW_WIDTH, C4_BASE_WINDOW_HEIGHT);

    C4_UI_Popup* winnerPopup = C4_UI_Container_Add_Popup(
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
    C4_UI_Button* playAgainButton = &winnerPopup->buttonGroup.buttons[0];
    C4_UI_Text_UpdateStr(&playAgainButton->text, "Play Again", game->renderer);
    playAgainButton->OnClickCallback = PlayAgainOnClick;
    
    C4_UI_Button* returnToMenuButton = &winnerPopup->buttonGroup.buttons[1];
    C4_UI_Text_UpdateStr(&returnToMenuButton->text, "Return to Menu", game->renderer);
    returnToMenuButton->OnClickCallback = ReturnToMenuOnClick;

    C4_UI_Popup_CenterInWindow(winnerPopup, game->renderer, C4_BASE_WINDOW_WIDTH, C4_BASE_WINDOW_HEIGHT);

    gameData.boardText = boardText;
    gameData.game = game;
    gameData.winnerPopup = winnerPopup;
}