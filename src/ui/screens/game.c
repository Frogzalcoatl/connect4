#include "Connect4/ui/screens/game.h"
#include "Connect4/game/random.h"
#include "Connect4/ui/fontManager.h"

C4_GameScreen* C4_GameScreen_Create(C4_Board* board, SDL_Renderer* renderer) {
    if (!board || !renderer) {
        return NULL;
    }
    C4_GameScreen* screen = calloc(1, sizeof(C4_GameScreen));
    if (!screen) {
        return NULL;
    }
    screen->board = board;
    screen->renderer = renderer;
    screen->testBoardText = C4_TextUIElement_Create(screen->renderer, "", C4_FontType_Regular, 140.f, (SDL_Color){255, 255, 255, 255}, 0.f, 0.f);
    if (!screen->testBoardText) {
        C4_GameScreen_Destroy(screen);
        return NULL;
    }
    C4_GameScreen_TestStrUpdate(screen);
    screen->menuButton = C4_Button_Create(
        screen->renderer, "Back", C4_FontType_Bold, 32.f,
        (SDL_FRect){0.f, 950.f, 400.f, 100.f},
        (C4_ButtonColorInfo){(SDL_Color){150, 150, 150, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_ButtonColorInfo){(SDL_Color){200, 200, 200, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_ButtonColorInfo){(SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255}},
        C4_ScreenChangeRequest_Menu
    );
    if (!screen->menuButton) {
        C4_GameScreen_Destroy(screen);
        return NULL;
    }
    C4_Button_CenterInWindow(screen->menuButton, C4_Axis_X);
    return screen;
}

void C4_GameScreen_Destroy(C4_GameScreen* screen) {
    if (!screen) {
        return;
    }
    C4_Button_Destroy(screen->menuButton);
    C4_TextUIElement_Destroy(screen->testBoardText);
    free(screen);
}

void C4_GameScreen_Draw(C4_GameScreen* screen) {
    if (!screen) {
        return;
    }
    C4_TextUIElement_Draw(screen->testBoardText, screen->renderer);
    C4_Button_Draw(screen->menuButton, screen->renderer);
}

void C4_GameScreen_TestStrUpdate(C4_GameScreen* screen) {
    if (!screen) {
        return;
    }
    char tempBuffer[512];
    C4_Board_UpdateTestStr(screen->board, tempBuffer, 100);
    C4_TextUIElement_ChangeStr(screen->testBoardText, tempBuffer);
    C4_TextUIElement_Refresh(screen->testBoardText, screen->renderer);
    C4_TextUIElement_CenterInWindow(screen->testBoardText, C4_Axis_X);
}

C4_ScreenChangeRequest C4_GameScreen_HandleKeyboardInput(C4_GameScreen* screen, SDL_Scancode scancode) {
    if (!screen) {
        return C4_ScreenChangeRequest_None;
    }
    // Temporary just for testing
    if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_7) {
        int column = scancode - SDL_SCANCODE_1;
        C4_Board_DoMove(screen->board, column);
        C4_GameScreen_TestStrUpdate(screen);
    }
    return C4_ScreenChangeRequest_None;
}

C4_ScreenChangeRequest C4_GameScreen_HandleMouseEvents(C4_GameScreen* screen, SDL_Event* event) {
    if (!screen || !event) {
        return C4_ScreenChangeRequest_None;
    }
    return C4_Button_HandleMouseEvents(screen->menuButton, event, screen->renderer);
}