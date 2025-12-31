#include "Connect4/game/screens/settings.h"
#include "Connect4/constants.h"

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

void C4_SetScreen_Settings(C4_Game* game) {
    if (game == NULL) {
        return;
    }
    C4_UI_Container* cont = &game->container;
    C4_UI_Container_Clear(cont);
    game->currentScreen = C4_ScreenType_Settings;
    C4_UI_Text* settingsTitle = C4_UI_Container_Add_Text(
        cont, "Settings", C4_FontType_Bold, C4_SCREEN_SETTINGS_TITLE_PT_SIZE,
        0.f, 0.f, 0, C4_UI_THEME_DEFAULT.textColor
    );
    C4_UI_CenterInWindow(&settingsTitle->destination, C4_Axis_X);
    C4_UI_ButtonGroup* buttonGroup = C4_UI_Container_Add_ButtonGroup(
        cont, (SDL_FRect){0.f, C4_SCREEN_SETTINGS_BUTTON_GROUP_YPOS,
        C4_SCREEN_SETTINGS_BUTTON_GROUP_WIDTH, C4_SCREEN_SETTINGS_BUTTON_GROUP_HEIGHT},
        SETTINGS_BUTTON_COUNT, C4_UI_ButtonGroup_Direction_Horizontal, 15, &C4_UI_THEME_DEFAULT
    );
    for (size_t i = 0; i < SETTINGS_BUTTON_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            buttonGroup, i, game->renderer, SETTINGS_BUTTONS_TEXT[i],
            C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT
        );
    }
    C4_UI_ButtonGroup_CenterInWindow(buttonGroup, C4_Axis_X);
    C4_UI_Popup* popup = C4_UI_Container_Add_Popup(
        cont, (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH, C4_SCREEN_SETTINGS_POPUP_HEIGHT},
        C4_UI_ButtonGroup_Direction_Horizontal, 2, 100.f, CONFIRM_MESSAGE_DEFAULT,
        &C4_UI_THEME_DEFAULT
    );
    for (size_t i = 0; i < SETTINGS_POPUP_BUTTONS_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            &popup->buttonGroup, i, game->renderer, SETTINGS_POPUP_BUTTONS_TEXT[i],
            C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT
        );
    }
    C4_UI_Popup_CenterInWindow(popup, game->renderer);
    C4_UI_NumberInput* boardWidthInput = C4_UI_Container_Add_NumberInput(
        cont, (SDL_FRect){1175.f, 175.f, 200.f, 100.f},
        2, 10, game->board->width, 20.f, 20.f, &C4_UI_THEME_DEFAULT
    );
    const float PT_SIZE = boardWidthInput->numberText.ptSize;
    C4_UI_Container_Add_Text(
        cont, "Board Width", C4_FontType_Regular,
        PT_SIZE, 475.f, 200.f, 0, C4_UI_THEME_DEFAULT.textColor
    );
    C4_UI_Container_Add_NumberInput(
        cont, (SDL_FRect){1175.f, 300.f, 200.f, 100.f},
        2, 6, game->board->height, 20.f, 20.f, &C4_UI_THEME_DEFAULT
    );
    C4_UI_Container_Add_Text(
        cont, "Board Height", C4_FontType_Regular, PT_SIZE,
        475.f, 325.f, 0, C4_UI_THEME_DEFAULT.textColor
    );
    C4_UI_Container_Add_NumberInput(
        cont, (SDL_FRect){1175.f, 425.f, 200.f, 100.f}, 2,
        C4_Board_GetMaxAmountToWin(game->board->width, game->board->height),
        game->board->amountToWin, 20.f, 20.f, &C4_UI_THEME_DEFAULT
    );
    C4_UI_Container_Add_Text(
        cont, "Amount to Win", C4_FontType_Regular, PT_SIZE,
        475.f, 450.f, 0, C4_UI_THEME_DEFAULT.textColor
    );
}