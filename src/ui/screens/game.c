#include "Connect4/ui/screens/game.h"
#include "Connect4/game/utils.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/assets/sounds.h"
#include <stdlib.h>

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
    screen->testBoardText = C4_UI_Text_Create(screen->renderer, "", C4_FontType_Regular, 140.f, (SDL_Color){255, 255, 255, 255}, 0.f, 0.f);
    if (!screen->testBoardText) {
        SDL_Log("Unable to allocate memory for test board text");
        C4_Screen_Game_Destroy(screen);
        return NULL;
    }
    C4_Screen_Game_TestStrUpdate(screen);
    screen->menuButton = C4_UI_Button_Create(
        screen->renderer, "Back", C4_FontType_Bold, 32.f,
        (SDL_FRect){0.f, 950.f, 400.f, 100.f},
        (C4_UI_ButtonColorInfo){(SDL_Color){150, 150, 150, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_UI_ButtonColorInfo){(SDL_Color){200, 200, 200, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_UI_ButtonColorInfo){(SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255}},
        C4_ScreenChangeRequest_Menu
    );
    if (!screen->menuButton) {
        SDL_Log("Unable to create menu button");
        C4_Screen_Game_Destroy(screen);
        return NULL;
    }
    C4_UI_Button_CenterInWindow(screen->menuButton, C4_Axis_X);
    return screen;
}

void C4_Screen_Game_Destroy(void* screenData) {
    if (!screenData) {
        SDL_Log("Tried to destroy NULL gamescreen");
        return;
    }
    C4_Screen_Game* screen = (C4_Screen_Game*)screenData;
    C4_UI_Button_Destroy(screen->menuButton);
    C4_UI_Text_Destroy(screen->testBoardText);
    free(screen);
}

void C4_Screen_Game_Draw(void* screenData) {
    if (!screenData) {
        SDL_Log("Game screen is NULL");
        return;
    }
    C4_Screen_Game* screen = (C4_Screen_Game*)screenData;
    C4_UI_Text_Draw(screen->testBoardText, screen->renderer);
    C4_UI_Button_Draw(screen->menuButton, screen->renderer);
}

void C4_Screen_Game_TestStrUpdate(C4_Screen_Game* screen) {
    if (!screen) {
        SDL_Log("Game screen is NULL");
        return;
    }
    char tempBuffer[512];
    C4_Board_UpdateTestStr(screen->board, tempBuffer, 100);
    C4_UI_Text_ChangeStr(screen->testBoardText, tempBuffer);
    C4_UI_Text_Refresh(screen->testBoardText, screen->renderer);
    C4_UI_Text_CenterInWindow(screen->testBoardText, C4_Axis_X);
}

C4_Screen_RequestChange C4_Screen_Game_HandleKeyboardInput(void* screenData, SDL_Scancode scancode) {
    if (!screenData) {
        SDL_Log("Game screen is NULL");
        return C4_ScreenChangeRequest_None;
    }
    C4_Screen_Game* screen = (C4_Screen_Game*)screenData;
    // Temporary just for testing
    if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_7) {
        int column = scancode - SDL_SCANCODE_1;
        int64_t atIndex = C4_Board_DoMove(screen->board, column);
        if (atIndex == -1) {
            return C4_ScreenChangeRequest_None;
        }
        // Reversed since currentPlayer would have been swapped already by doMove
        if (screen->board->currentPlayer == C4_SlotState_Player2) {
            C4_PlaySound(C4_SoundEffect_Player1Place);
        } else {
            C4_PlaySound(C4_SoundEffect_Player2Place);
        }
        C4_SlotState winnerCheckResult = C4_Board_GetWinner(screen->board, atIndex);
        SDL_Log("winnerCheckResult: %i", winnerCheckResult);
        C4_Screen_Game_TestStrUpdate(screen);
    }
    return C4_ScreenChangeRequest_None;
}

C4_Screen_RequestChange C4_Screen_Game_HandleMouseEvents(void* screenData, SDL_Event* event) {
    if (!screenData || !event) {
        SDL_Log("Game screen and/or event is NULL");
        return C4_ScreenChangeRequest_None;
    }
    C4_Screen_Game* screen = (C4_Screen_Game*)screenData;
    return C4_UI_Button_HandleMouseEvents(screen->menuButton, event, screen->renderer);
}