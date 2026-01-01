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

#define GAME_POPUP_BUTTONS_COUNT 2
typedef enum {
    Game_PopupButtons_PlayAgain,
    Game_PopupButtons_ReturnToMenu
} Game_PopupButtons;
static const char GAME_POPUP_BUTTONS_TEXT[GAME_POPUP_BUTTONS_COUNT][16] = {
    "Play Again",
    "Return to Menu"
};
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
    if (game == NULL) {
        return;
    }
    C4_UI_Container* cont = &game->container;
    C4_UI_Container_Clear(cont);
    game->currentScreen = C4_ScreenType_Game;

    C4_UI_Text* boardText = C4_UI_Container_Add_Text(
        cont, "", C4_FontType_Regular, C4_SCREEN_GAME_TEST_TEXT_PT_SIZE,
        0.f, 0.f, 0, C4_UI_THEME_DEFAULT.textColor
    );
    C4_Board_UpdateTestStr(game->board, boardText->str, sizeof(boardText->str));
    C4_UI_Text_ReloadTexture(boardText, game->renderer);
    C4_UI_CenterInWindow(&boardText->destination, C4_Axis_X);

    C4_UI_Button* backButton = C4_UI_Container_Add_Button(
        cont, C4_SCREEN_GAME_BACK_BUTTON_RECT, "Back",
        &C4_UI_THEME_DEFAULT, BackOnClick, NULL
    );
    C4_UI_Button_CenterInWindow(backButton, C4_Axis_X);

    C4_UI_Popup* winnerPopup = C4_UI_Container_Add_Popup(
        cont, (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH,
        C4_SCREEN_SETTINGS_POPUP_HEIGHT}, C4_UI_ButtonGroup_Direction_Horizontal,
        GAME_POPUP_BUTTONS_COUNT, 100.f, "", &C4_UI_THEME_DEFAULT
    );
    C4_UI_ButtonGroup_SetButtonIndex(
        &winnerPopup->buttonGroup, 0, game->renderer, "Play Again",
        C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT, PlayAgainOnClick, NULL
    );
    C4_UI_ButtonGroup_SetButtonIndex(
        &winnerPopup->buttonGroup, 1, game->renderer, "Return to Menu",
        C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT, ReturnToMenuOnClick, NULL
    );
    C4_UI_Popup_CenterInWindow(winnerPopup, game->renderer);

    gameData.boardText = boardText;
    gameData.game = game;
    gameData.winnerPopup = winnerPopup;
}