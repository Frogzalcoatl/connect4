#include "Connect4/ui/screens/game.h"
#include "Connect4/game/utils.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define GAME_POPUP_BUTTONS_COUNT 2
typedef enum {
    Game_PopupButtons_PlayAgain,
    Game_PopupButtons_ReturnToMenu
} Game_PopupButtons;
static const char GAME_POPUP_BUTTONS_TEXT[GAME_POPUP_BUTTONS_COUNT][16] = {
    "Play Again",
    "Return to Menu"
};

C4_Screen_Game* C4_Screen_Game_Create(SDL_Renderer* renderer, C4_Board* board) {
    if (!board || !renderer) {
        SDL_Log("Unable to create game screen. renderer and/or board is NULL");
        return NULL;
    }
    C4_Screen_Game* screen = calloc(1, sizeof(C4_Screen_Game));
    if (!screen) {
        SDL_Log("Unable to allocate memory for game screen");
        return NULL;
    }
    screen->board = board;
    screen->renderer = renderer;
    if (
        !C4_UI_Text_InitProperties(
            &screen->testBoardText, screen->renderer, "", C4_FontType_Regular,
            C4_SCREEN_GAME_TEST_TEXT_PT_SIZE, 0.f, 0.f, 0, C4_UI_THEME_DEFAULT.textColor
        )
    ) {
        C4_Screen_Game_Destroy(screen);
        return NULL;
    }
    C4_Screen_Game_TestStrUpdate(screen);
    if (
        !C4_UI_Button_InitProperties(
            &screen->menuButton, screen->renderer, "Back", C4_SCREEN_GAME_BACK_BUTTON_RECT, C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT
        )
    ) {
        C4_Screen_Game_Destroy(screen);
        return NULL;
    }
    C4_UI_Button_CenterInWindow(&screen->menuButton, C4_Axis_X);
    if (
        !C4_UI_Popup_InitProperties(
            &screen->winnerPopup, screen->renderer, (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH, C4_SCREEN_SETTINGS_POPUP_HEIGHT},
            C4_UI_ButtonGroup_Direction_Horizontal, GAME_POPUP_BUTTONS_COUNT, 100.f, "", &C4_UI_THEME_DEFAULT
        )
    ) {
        C4_Screen_Game_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < GAME_POPUP_BUTTONS_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            &screen->winnerPopup.buttonGroup, i, screen->renderer, GAME_POPUP_BUTTONS_TEXT[i],
            C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT
        );
    }
    C4_UI_Popup_CenterInWindow(&screen->winnerPopup);
    return screen;
}

void C4_Screen_Game_Destroy(void* screenData) {
    if (!screenData) {
        SDL_Log("Tried to destroy NULL gamescreen");
        return;
    }
    C4_Screen_Game* screen = (C4_Screen_Game*)screenData;
    C4_UI_Popup_FreeResources(&screen->winnerPopup);
    C4_UI_Button_FreeResources(&screen->menuButton);
    C4_UI_Text_FreeResources(&screen->testBoardText);
    free(screen);
}

void C4_Screen_Game_Draw(void* screenData) {
    if (!screenData) {
        SDL_Log("Game screen is NULL");
        return;
    }
    C4_Screen_Game* screen = (C4_Screen_Game*)screenData;
    C4_UI_Text_Draw(&screen->testBoardText, screen->renderer);
    C4_UI_Button_Draw(&screen->menuButton, screen->renderer);
    C4_UI_Popup_Draw(&screen->winnerPopup);
}

void C4_Screen_Game_TestStrUpdate(C4_Screen_Game* screen) {
    if (!screen) {
        SDL_Log("Game screen is NULL");
        return;
    }
    char tempBuffer[512];
    C4_Board_UpdateTestStr(screen->board, tempBuffer, 100);
    C4_UI_Text_ChangeStr(&screen->testBoardText, tempBuffer);
    C4_UI_Text_Refresh(&screen->testBoardText, screen->renderer);
    C4_UI_CenterInWindow(&screen->testBoardText.destination, C4_Axis_X);
}

static void C4_Screen_Game_UpdateWinnerPopupText(C4_Screen_Game* screen, C4_SlotState winner) {
    if (winner != C4_SlotState_Player1 && winner != C4_SlotState_Player2) {
        return;
    }
    char newStr[64];
    snprintf(
        newStr,
        sizeof(newStr),
        "Game Over!\nWinner: Player %c",
        C4_Board_GetCharForState(winner)
    );
    C4_UI_Text_ChangeStr(
        &screen->winnerPopup.message,
        newStr
    );
    C4_UI_Text_Refresh(&screen->winnerPopup.message, screen->renderer);
}

void C4_Screen_Game_HandleKeyboardInput(void* screenData, SDL_Scancode scancode) {
    if (!screenData) {
        SDL_Log("Game screen is NULL");
        return;
    }
    if (scancode == SDL_SCANCODE_ESCAPE) {
        C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
        return;
    }
    C4_Screen_Game* screen = (C4_Screen_Game*)screenData;
    // Temporary just for testing
    if (
        scancode >= SDL_SCANCODE_1 &&
        scancode <= SDL_SCANCODE_7 &&
        !screen->winnerPopup.isShowing
    ) {
        int column = scancode - SDL_SCANCODE_1;
        int64_t atIndex = C4_Board_DoMove(screen->board, (uint8_t)column);
        if (atIndex == -1) {
            return;
        }
        // Reversed since currentPlayer would have been swapped already by doMove
        if (screen->board->currentPlayer == C4_SlotState_Player2) {
            C4_PushEvent_SoundRequest(C4_SoundEffect_Player1Place);
        } else {
            C4_PushEvent_SoundRequest(C4_SoundEffect_Player2Place);
        }
        C4_SlotState winnerCheckResult = C4_Board_GetWinner(screen->board, (size_t)atIndex);
        if (winnerCheckResult == C4_SlotState_Player1 || winnerCheckResult == C4_SlotState_Player2) {
            C4_Screen_Game_UpdateWinnerPopupText(screen, winnerCheckResult);
            C4_PushEvent_GameOver(winnerCheckResult);
            screen->winnerPopup.isShowing = true;
        }
        C4_Screen_Game_TestStrUpdate(screen);
    }
}

void C4_Screen_Game_HandleMouseEvents(void* screenData, SDL_Event* event) {
    if (!screenData || !event) {
        SDL_Log("Game screen and/or event is NULL");
        return;
    }
    C4_Screen_Game* screen = (C4_Screen_Game*)screenData;
    if (screen->winnerPopup.isShowing) {
        switch (C4_UI_Popup_HandleMouseEvents(&screen->winnerPopup, event)) {
            case Game_PopupButtons_PlayAgain: {
                C4_Board_Reset(screen->board);
                C4_Screen_Game_TestStrUpdate(screen);
                screen->winnerPopup.isShowing = false;
            }; break;
            case Game_PopupButtons_ReturnToMenu: {
                C4_Board_Reset(screen->board);
                C4_Screen_Game_TestStrUpdate(screen);
                C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
            }
        }
        return;
    }
    if (C4_UI_Button_HandleMouseEvents(&screen->menuButton, event)) {
        C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
    }
}