#include "Connect4/game/screens/settings.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"

typedef struct {
    C4_Game* game;
    C4_UI_NumberInput* widthInput;
    C4_UI_NumberInput* heightInput;
    C4_UI_NumberInput* winAmountInput;
    C4_UI_Popup* confirmPopup;
    C4_UI_Button* applyButton;
} C4_SettingsScreenData;
static C4_SettingsScreenData settingsData;

static bool IsUpdatingBoard() {
    C4_Board* board = settingsData.game->board;
    return (
        board->width != settingsData.widthInput->currentValue ||
        board->height != settingsData.heightInput->currentValue ||
        board->amountToWin != settingsData.winAmountInput->currentValue
    );
}

#define BUTTON_GROUP_COUNT 2
typedef enum {
    Settings_ButtonGroup_Apply,
    Settings_ButtonGroup_Cancel
} Settings_ButtonGroup;
static const char SETTINGS_BUTTONS_TEXT[BUTTON_GROUP_COUNT][16] = {
    "Apply",
    "Cancel",
};
static const char CONFIRM_MESSAGE_DEFAULT[] = "Are you sure you want to apply these settings?";
static const char CONFIRM_MESSAGE_BOARD_NOT_EMPTY[] = "Are you sure you want to apply these settings? This will clear the current game.";
static void ApplyOnClick(void* context) {
    (void)context;
    C4_SettingsScreenData* data = &settingsData;
    if (!IsUpdatingBoard(data)) {
        return;
    }
    bool isBoardEmpty = C4_Board_IsEmpty(data->game->board);
    if (isBoardEmpty && strcmp(data->confirmPopup->message.str, CONFIRM_MESSAGE_DEFAULT)) {
        C4_UI_Text_UpdateStr(&data->confirmPopup->message, CONFIRM_MESSAGE_DEFAULT, data->game->renderer);
    }
    if (!isBoardEmpty && strcmp(data->confirmPopup->message.str, CONFIRM_MESSAGE_BOARD_NOT_EMPTY)) {
        C4_UI_Text_UpdateStr(&data->confirmPopup->message, CONFIRM_MESSAGE_BOARD_NOT_EMPTY, data->game->renderer);
    }
    data->confirmPopup->isShowing = true;
}
static void CancelOnClick(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
}
static const C4_UI_Callback BUTTON_GROUP_ON_CLICKS[BUTTON_GROUP_COUNT] = {
    ApplyOnClick,
    CancelOnClick
};

#define POPUP_BUTTON_COUNT 2
typedef enum {
    Settings_PopupButtons_Yes,
    Settings_PopupButtons_Back
} Settings_PopupButtons;
static const char POPUP_BUTTON_TEXT[POPUP_BUTTON_COUNT][8] = {
    "Yes",
    "Back"
};
static void PopupYesOnClick(void* context) {
    (void)context;
    C4_SettingsScreenData* data = &settingsData;
    C4_Board_ChangeSize(data->game->board, (uint8_t)data->widthInput->currentValue, (uint8_t)data->heightInput->currentValue);
    data->game->board->amountToWin = (uint8_t)data->winAmountInput->currentValue;
    C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
}
static void PopupBackOnClick(void* context) {
    (void)context;
    settingsData.confirmPopup->isShowing = false;
}
static const C4_UI_Callback POPUP_ON_CLICKS[BUTTON_GROUP_COUNT] = {
    PopupYesOnClick,
    PopupBackOnClick
};

static void ApplyButtonInactiveHandler(void) {
    settingsData.applyButton->isActive = IsUpdatingBoard();
}
static void ChangeAmountToWinMax(void) {
    C4_UI_NumberInput_ChangeMax(
        settingsData.winAmountInput,
        C4_Board_GetMaxAmountToWin(
            (uint8_t)settingsData.widthInput->currentValue,
            (uint8_t)settingsData.heightInput->currentValue
        ),
        settingsData.game->renderer
    );
}
static void WidthIncrementOnClick(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericIncrementOnClick(settingsData.widthInput);
    ChangeAmountToWinMax();
    ApplyButtonInactiveHandler();
}
static void WidthDecrementOnClick(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericDecrementOnClick(settingsData.widthInput);
    ChangeAmountToWinMax();
    ApplyButtonInactiveHandler();
}
static void HeightIncrementOnClick(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericIncrementOnClick(settingsData.heightInput);
    ChangeAmountToWinMax();
    ApplyButtonInactiveHandler();
}
static void HeightDecrementOnClick(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericDecrementOnClick(settingsData.heightInput);
    ChangeAmountToWinMax();
    ApplyButtonInactiveHandler();
}
static void AmountToWinIncrementOnClick(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericIncrementOnClick(settingsData.winAmountInput);
    ApplyButtonInactiveHandler();
}
static void AmountToWinDecrementOnClick(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericDecrementOnClick(settingsData.winAmountInput);
    ApplyButtonInactiveHandler();
}

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
        BUTTON_GROUP_COUNT, C4_UI_ButtonGroup_Direction_Horizontal, 15, &C4_UI_THEME_DEFAULT
    );
    for (size_t i = 0; i < BUTTON_GROUP_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            buttonGroup, i, game->renderer, SETTINGS_BUTTONS_TEXT[i],
            C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT,
            BUTTON_GROUP_ON_CLICKS[i], NULL
        );
    }
    C4_UI_Button* applyButton = &buttonGroup->buttons[0];
    applyButton->isActive = false;
    C4_UI_ButtonGroup_CenterInWindow(buttonGroup, C4_Axis_X);

    C4_UI_Popup* confirmPopup = C4_UI_Container_Add_Popup(
        cont, (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH, C4_SCREEN_SETTINGS_POPUP_HEIGHT},
        C4_UI_ButtonGroup_Direction_Horizontal, 2, 100.f, CONFIRM_MESSAGE_DEFAULT,
        &C4_UI_THEME_DEFAULT
    );
    for (size_t i = 0; i < POPUP_BUTTON_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            &confirmPopup->buttonGroup, i, game->renderer, POPUP_BUTTON_TEXT[i],
            C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT,
            POPUP_ON_CLICKS[i], NULL
        );
    }
    C4_UI_Popup_CenterInWindow(confirmPopup, game->renderer);

    C4_UI_NumberInput* boardWidthInput = C4_UI_Container_Add_NumberInput(
        cont, (SDL_FRect){1175.f, 175.f, 200.f, 100.f},
        2, 10, game->board->width, 20.f, 20.f, &C4_UI_THEME_DEFAULT,
        WidthIncrementOnClick, NULL, WidthDecrementOnClick, NULL
    );

    const float PT_SIZE = boardWidthInput->numberText.ptSize;

    C4_UI_Container_Add_Text(
        cont, "Board Width", C4_FontType_Regular,
        PT_SIZE, 475.f, 200.f, 0, C4_UI_THEME_DEFAULT.textColor
    );

    C4_UI_NumberInput* boardHeightInput = C4_UI_Container_Add_NumberInput(
        cont, (SDL_FRect){1175.f, 300.f, 200.f, 100.f},
        2, 6, game->board->height, 20.f, 20.f, &C4_UI_THEME_DEFAULT,
        HeightIncrementOnClick, NULL, HeightDecrementOnClick, NULL
    );

    C4_UI_Container_Add_Text(
        cont, "Board Height", C4_FontType_Regular, PT_SIZE,
        475.f, 325.f, 0, C4_UI_THEME_DEFAULT.textColor
    );

    C4_UI_NumberInput* winAmountInput = C4_UI_Container_Add_NumberInput(
        cont, (SDL_FRect){1175.f, 425.f, 200.f, 100.f}, 2,
        C4_Board_GetMaxAmountToWin(game->board->width, game->board->height),
        game->board->amountToWin, 20.f, 20.f, &C4_UI_THEME_DEFAULT,
        AmountToWinIncrementOnClick, NULL, AmountToWinDecrementOnClick, NULL
    );

    C4_UI_Container_Add_Text(
        cont, "Amount to Win", C4_FontType_Regular, PT_SIZE,
        475.f, 450.f, 0, C4_UI_THEME_DEFAULT.textColor
    );

    settingsData.applyButton = applyButton;
    settingsData.confirmPopup = confirmPopup;
    settingsData.game = game;
    settingsData.heightInput = boardHeightInput;
    settingsData.widthInput = boardWidthInput;
    settingsData.winAmountInput = winAmountInput;
}