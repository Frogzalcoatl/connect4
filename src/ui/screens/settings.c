#include "Connect4/ui/screens/settings.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include <stdlib.h>
#include <stdio.h>

#define SETTINGS_BUTTON_COUNT 2
typedef enum {
    Settings_ButtonGroup_Apply,
    Settings_ButtonGroup_Cancel
} Settings_ButtonGroup;
static const char SETTINGS_BUTTONS_TEXT[SETTINGS_BUTTON_COUNT][16] = {
    "Apply",
    "Cancel",
};

#define SETTINGS_POPUP_BUTTONS_COUNT 2
typedef enum {
    Settings_PopupButtons_Yes,
    Settings_PopupButtons_Back
} Settings_PopupButtons;
static const char SETTINGS_POPUP_BUTTONS_TEXT[SETTINGS_POPUP_BUTTONS_COUNT][8] = {
    "Yes",
    "Back"
};

static const char CONFIRM_MESSAGE_DEFAULT[64] = "Are you sure you want to apply these settings?";
static const char CONFIRM_MESSAGE_BOARD_NOT_EMPTY[128] = "Are you sure you want to apply these settings? This will clear the current game.";

C4_Screen_Settings* C4_Screen_Settings_Create(SDL_Renderer* renderer, SDL_Window* window, C4_Board* board) {
    if (!renderer) {
        SDL_Log("Unable to create settings screen. SDL renderer is NULL");
        return NULL;
    }
    if (!window) {
        SDL_Log("Unable to create settings screen. Window is NULL");
        return NULL;
    }
    if (!board) {
        SDL_Log("Unable to create settings screen. Board is NULL");
        return NULL;
    }
    C4_Screen_Settings* screen = calloc(1, sizeof(C4_Screen_Settings));
    if (!screen) {
        SDL_Log("Unable to allocate memory for settings screen");
        return NULL;
    }
    screen->window = window;
    screen->renderer = renderer;
    screen->board = board;
    if (!C4_UI_Text_InitProperties(&screen->title, screen->renderer, "Settings", C4_FontType_Bold, C4_SCREEN_SETTINGS_TITLE_PT_SIZE, 0.f, 0.f, 0, C4_UI_THEME_DEFAULT.textColor)) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    C4_UI_CenterInWindow(&screen->title.destination, C4_Axis_X);
    if (
        !C4_UI_ButtonGroup_InitProperties(
            &screen->buttonGroup, renderer,
            (SDL_FRect){0.f, C4_SCREEN_SETTINGS_BUTTON_GROUP_YPOS, C4_SCREEN_SETTINGS_BUTTON_GROUP_WIDTH, C4_SCREEN_SETTINGS_BUTTON_GROUP_HEIGHT},
            SETTINGS_BUTTON_COUNT, C4_UI_ButtonGroup_Direction_Horizontal, 15, &C4_UI_THEME_DEFAULT
        )
    ) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < SETTINGS_BUTTON_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(&screen->buttonGroup, i, screen->renderer, SETTINGS_BUTTONS_TEXT[i], C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT);
    }
    C4_UI_ButtonGroup_CenterInWindow(&screen->buttonGroup, C4_Axis_X);
    if (!C4_UI_Popup_InitProperties(
        &screen->confirmationPopup, screen->renderer,
        (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH, C4_SCREEN_SETTINGS_POPUP_HEIGHT},
        C4_UI_ButtonGroup_Direction_Horizontal, 2, 100.f,
        CONFIRM_MESSAGE_DEFAULT, &C4_UI_THEME_DEFAULT
    )) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < SETTINGS_POPUP_BUTTONS_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(&screen->confirmationPopup.buttonGroup, i, screen->renderer, SETTINGS_POPUP_BUTTONS_TEXT[i], C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT);
    }
    C4_UI_Popup_CenterInWindow(&screen->confirmationPopup);
    if (
        !C4_UI_NumberInput_InitProperties(&screen->boardWidthInput, screen->renderer, (SDL_FRect){1175.f, 175.f, 200.f, 100.f}, 2, 10, board->width, 20.f, 20.f, &C4_UI_THEME_DEFAULT)
    ) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    float settingsTextPtSize = screen->boardWidthInput.numberText.ptSize;
    if (
        !C4_UI_Text_InitProperties(
            &screen->boardWidthText, renderer, "Board Width", C4_FontType_Regular, settingsTextPtSize, 475.f, 200.f, 0, C4_UI_THEME_DEFAULT.textColor
        )
    ) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    if (
        !C4_UI_NumberInput_InitProperties(
            &screen->boardHeightInput, screen->renderer, (SDL_FRect){1175.f, 300.f, 200.f, 100.f}, 2, 6, board->height, 20.f, 20.f, &C4_UI_THEME_DEFAULT
        )
    ) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    if (
        !C4_UI_Text_InitProperties(
            &screen->boardHeightText, renderer, "Board Height", C4_FontType_Regular, settingsTextPtSize, 475.f, 325.f, 0, C4_UI_THEME_DEFAULT.textColor
        )
    ) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    if (
        !C4_UI_NumberInput_InitProperties(
            &screen->amountToWinInput, renderer, (SDL_FRect){1175.f, 425.f, 200.f, 100.f}, 2,
            C4_Board_GetMaxAmountToWin(board->width, board->height), board->amountToWin, 20.f, 20.f, &C4_UI_THEME_DEFAULT
        )
    ) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    if (
        !C4_UI_Text_InitProperties(
            &screen->amountToWinText, renderer, "Amount to Win", C4_FontType_Regular, settingsTextPtSize, 475.f, 450.f, 0, C4_UI_THEME_DEFAULT.textColor
        )
    ) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    return screen;
}

void C4_Screen_Settings_Destroy(void* screenData) {
    if (!screenData) {
        SDL_Log("Settings screen is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    C4_UI_Text_FreeResources(&screen->amountToWinText);
    C4_UI_NumberInput_FreeResources(&screen->amountToWinInput);
    C4_UI_Text_FreeResources(&screen->boardHeightText);
    C4_UI_NumberInput_FreeResources(&screen->boardHeightInput);
    C4_UI_Text_FreeResources(&screen->boardWidthText);
    C4_UI_NumberInput_FreeResources(&screen->boardWidthInput);
    C4_UI_ButtonGroup_FreeResources(&screen->buttonGroup);
    C4_UI_Popup_FreeResources(&screen->confirmationPopup);
    C4_UI_Text_FreeResources(&screen->title);
    free(screen);
}

void C4_Screen_Settings_Draw(void* screenData) {
    if (!screenData) {
        SDL_Log("Settings screen is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    C4_UI_ButtonGroup_Draw(&screen->buttonGroup, screen->renderer);
    C4_UI_Text_Draw(&screen->title, screen->renderer);
    C4_UI_NumberInput_Draw(&screen->boardWidthInput, screen->renderer);
    C4_UI_Text_Draw(&screen->boardWidthText, screen->renderer);
    C4_UI_NumberInput_Draw(&screen->boardHeightInput, screen->renderer);
    C4_UI_Text_Draw(&screen->boardHeightText, screen->renderer);
    C4_UI_NumberInput_Draw(&screen->amountToWinInput, screen->renderer);
    C4_UI_Text_Draw(&screen->amountToWinText, screen->renderer);
    C4_UI_Popup_Draw(&screen->confirmationPopup);
}

void C4_Screen_Settings_HandleKeyboardInput(void* screenData, SDL_Scancode scancode) {
    if (!screenData) {
        SDL_Log("Settings screen is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    if (scancode == SDL_SCANCODE_ESCAPE) {
        if (screen->confirmationPopup.isShowing) {
            screen->confirmationPopup.isShowing = false;
        } else {
            C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
        }
    }
}

void C4_Screen_Settings_HandleMouseEvents(void* screenData, SDL_Event* event) {
    if (!screenData || !event) {
        SDL_Log("Settings screen and/or event is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    switch (C4_UI_Popup_HandleMouseEvents(&screen->confirmationPopup, event)) {
        case Settings_PopupButtons_Yes: {  
            if (
                screen->board->width != screen->boardWidthInput.currentValue ||
                screen->board->height != screen->boardHeightInput.currentValue
            ) {
                C4_Board_ChangeSize(screen->board, (uint8_t)screen->boardWidthInput.currentValue, (uint8_t)screen->boardHeightInput.currentValue);
            }
            screen->board->amountToWin = (uint8_t)screen->amountToWinInput.currentValue;
            screen->confirmationPopup.isShowing = false;
            C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
        }; return;
        case Settings_PopupButtons_Back: {
            screen->confirmationPopup.isShowing = false;
        }; return;
        default: break;
    }
    if (screen->confirmationPopup.isShowing) {
        return;
    }
    switch (C4_UI_ButtonGroup_HandleMouseEvents(&screen->buttonGroup, event, screen->renderer)) {
        case Settings_ButtonGroup_Apply: {
            bool isUpdatingBoardSize = (
                screen->board->width != screen->boardWidthInput.currentValue ||
                screen->board->height != screen->boardHeightInput.currentValue
            );
            bool isUpdatingAmountToWin = screen->board->amountToWin != screen->amountToWinInput.currentValue;
            if (!isUpdatingBoardSize && !isUpdatingAmountToWin) {
                break;
            }
            screen->confirmationPopup.isShowing = true;
            screen->buttonGroup.buttons[0].background.color = screen->buttonGroup.buttons[0].defaultColors.background;
            bool isBoardEmpty = C4_Board_IsEmpty(screen->board);
            if (isBoardEmpty && strcmp(screen->confirmationPopup.message.str, CONFIRM_MESSAGE_DEFAULT)) {
                C4_UI_Text_UpdateStr(&screen->confirmationPopup.message, CONFIRM_MESSAGE_DEFAULT, screen->renderer);
            }
            if (!isBoardEmpty && strcmp(screen->confirmationPopup.message.str, CONFIRM_MESSAGE_BOARD_NOT_EMPTY)) {
                C4_UI_Text_UpdateStr(&screen->confirmationPopup.message, CONFIRM_MESSAGE_BOARD_NOT_EMPTY, screen->renderer);
            }
        }; break;
        case Settings_ButtonGroup_Cancel: {
            C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
        }; break;
        default: break;
    }
    if (
        C4_UI_NumberInput_HandleMouseEvents(&screen->boardWidthInput, event, screen->renderer) ||
        C4_UI_NumberInput_HandleMouseEvents(&screen->boardHeightInput, event, screen->renderer)
    ) {
        C4_UI_NumberInput_ChangeMax(
            &screen->amountToWinInput,
            C4_Board_GetMaxAmountToWin((uint8_t)screen->boardWidthInput.currentValue, (uint8_t)screen->boardHeightInput.currentValue),
            screen->renderer
        );
    }
    C4_UI_NumberInput_HandleMouseEvents(&screen->amountToWinInput, event, screen->renderer);
}