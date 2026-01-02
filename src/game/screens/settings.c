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

static bool IsUpdatingBoard(void) {
    C4_Board* board = settingsData.game->board;
    return (
        board->width != settingsData.widthInput->currentValue ||
        board->height != settingsData.heightInput->currentValue ||
        board->amountToWin != settingsData.winAmountInput->currentValue
    );
}

#define BUTTON_GROUP_COUNT 2
static const char SETTINGS_BUTTONS_TEXT[BUTTON_GROUP_COUNT][16] = {
    "Apply",
    "Cancel",
};
static const char CONFIRM_MESSAGE_DEFAULT[] = "Are you sure you want to apply these settings?";
static const char CONFIRM_MESSAGE_BOARD_NOT_EMPTY[] = "Are you sure you want to apply these settings? This will clear the current game.";
static void ApplyOnClick(void* context) {
    (void)context;
    C4_SettingsScreenData* data = &settingsData;
    if (!IsUpdatingBoard()) {
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

static void UniversalWhilePressedFuncs() {
    ChangeAmountToWinMax();
    ApplyButtonInactiveHandler();
    C4_UI_NumberInput_GenericButtonInactiveHandler(settingsData.winAmountInput);
    C4_PlaySound(C4_SoundEffect_ButtonClick);
}

static void WidthIncrementWhilePressed(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericIncrementCallback(settingsData.widthInput);
    C4_UI_NumberInput_GenericButtonInactiveHandler(settingsData.widthInput);
    UniversalWhilePressedFuncs();
}

static void WidthDecrementWhilePressed(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericDecrementCallback(settingsData.widthInput);
    C4_UI_NumberInput_GenericButtonInactiveHandler(settingsData.widthInput);
    UniversalWhilePressedFuncs();
}

static void HeightIncrementWhilePressed(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericIncrementCallback(settingsData.heightInput);
    C4_UI_NumberInput_GenericButtonInactiveHandler(settingsData.heightInput);
    UniversalWhilePressedFuncs();
}

static void HeightDecrementWhilePressed(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericDecrementCallback(settingsData.heightInput);
    C4_UI_NumberInput_GenericButtonInactiveHandler(settingsData.heightInput);
    UniversalWhilePressedFuncs();
}

static void WinAmountIncrementWhilePressed(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericIncrementCallback(settingsData.winAmountInput);
    UniversalWhilePressedFuncs();
}

static void WinAmountDecrementWhilePressed(void* context) {
    (void)context;
    C4_UI_NumberInput_GenericDecrementCallback(settingsData.winAmountInput);
    UniversalWhilePressedFuncs();
}

void C4_SetScreen_Settings(C4_Game* game) {
    if (!game) {
        return;
    }
    C4_UI_Container* cont = &game->container;
    C4_UI_Container_Clear(cont);
    game->currentScreen = C4_ScreenType_Settings;

    C4_UI_Text* settingsTitle = C4_UI_Container_Add_Text(
        cont, &(C4_UI_Text_Config){
            .str = "Settings",
            .font = game->fontBold,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = 100.f,
            .destinationX = 0.f,
            .destinationY = 0.f,
            .wrapWidth = 0
        }
    );
    C4_UI_CenterInWindow(&settingsTitle->destination, C4_Axis_X, C4_BASE_WINDOW_WIDTH, C4_BASE_WINDOW_HEIGHT);

    C4_UI_ButtonGroup* buttonGroup = C4_UI_Container_Add_ButtonGroup(
        cont, &(C4_UI_ButtonGroup_Config){
            .destination = (SDL_FRect){0.f, 900.f, 900.f, 100.f},
            .count = BUTTON_GROUP_COUNT,
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .margin = 15,
            .font = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    for (size_t i = 0; i < BUTTON_GROUP_COUNT; i++) {
        C4_UI_Button* btn = &buttonGroup->buttons[i];
        C4_UI_Text_UpdateStr(&btn->text, SETTINGS_BUTTONS_TEXT[i], game->renderer);
        btn->OnClickCallback = BUTTON_GROUP_ON_CLICKS[i];
    }
    C4_UI_Button* applyButton = &buttonGroup->buttons[0];
    applyButton->isActive = false;
    C4_UI_ButtonGroup_CenterInWindow(buttonGroup, C4_Axis_X, C4_BASE_WINDOW_WIDTH, C4_BASE_WINDOW_HEIGHT);

    C4_UI_Popup* confirmPopup = C4_UI_Container_Add_Popup(
        cont, &(C4_UI_Popup_Config){
            .destination = (SDL_FRect){0.f, 0.f, 800.f, 400.f},
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .buttonCount = POPUP_BUTTON_COUNT,
            .buttonGroupHeight = 100.f,
            .message = CONFIRM_MESSAGE_DEFAULT,
            .messageFont = game->fontRegular,
            .buttonFont = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    for (size_t i = 0; i < POPUP_BUTTON_COUNT; i++) {
        C4_UI_Button* btn = &confirmPopup->buttonGroup.buttons[i];
        C4_UI_Text_UpdateStr(&btn->text, POPUP_BUTTON_TEXT[i], game->renderer);
        btn->OnClickCallback = POPUP_ON_CLICKS[i];
    }
    C4_UI_Popup_CenterInWindow(confirmPopup, game->renderer, C4_BASE_WINDOW_WIDTH, C4_BASE_WINDOW_HEIGHT);

    C4_UI_NumberInput* boardWidthInput = C4_UI_Container_Add_NumberInput(
        cont, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 175.f, 200.f, 100.f},
            .min = 2,
            .max = 10,
            .startingValue = game->board->width,
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    C4_UI_Button* boardWidthIncrement = &boardWidthInput->buttonGroup.buttons[0];
    boardWidthIncrement->WhilePressedCallback = WidthIncrementWhilePressed;
    C4_UI_Button* boardWidthDecrement = &boardWidthInput->buttonGroup.buttons[1];
    boardWidthDecrement->WhilePressedCallback = WidthDecrementWhilePressed;

    const float PT_SIZE = boardWidthInput->numberText.ptSize;

    C4_UI_Container_Add_Text(
        cont, &(C4_UI_Text_Config){
            .str = "Board Width",
            .font = game->fontRegular,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = PT_SIZE,
            .destinationX = 475.f,
            .destinationY = 200.f,
            .wrapWidth = 0
        }
    );

    C4_UI_NumberInput* boardHeightInput = C4_UI_Container_Add_NumberInput(
        cont, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 300.f, 200.f, 100.f},
            .min = 2,
            .max = 6,
            .startingValue = game->board->height,
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    C4_UI_Button* boardHeightIncrement = &boardHeightInput->buttonGroup.buttons[0];
    boardHeightIncrement->WhilePressedCallback = HeightIncrementWhilePressed;
    C4_UI_Button* boardHeightDecrement = &boardHeightInput->buttonGroup.buttons[1];
    boardHeightDecrement->WhilePressedCallback = HeightDecrementWhilePressed;

    C4_UI_Container_Add_Text(
        cont, &(C4_UI_Text_Config){
            .str = "Board Height",
            .font = game->fontRegular,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = PT_SIZE,
            .destinationX = 475.f,
            .destinationY = 325.f,
            .wrapWidth = 0
        }
    );

    C4_UI_NumberInput* winAmountInput = C4_UI_Container_Add_NumberInput(
        cont, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 425.f, 200.f, 100.f},
            .min = 2,
            .max = C4_Board_GetMaxAmountToWin(game->board->width, game->board->height),
            .startingValue = game->board->amountToWin,
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    C4_UI_Button* winAmountIncrement = &winAmountInput->buttonGroup.buttons[0];
    winAmountIncrement->WhilePressedCallback = WinAmountIncrementWhilePressed;
    C4_UI_Button* winAmountDecrement = &winAmountInput->buttonGroup.buttons[1];
    winAmountDecrement->WhilePressedCallback = WinAmountDecrementWhilePressed;

    C4_UI_Container_Add_Text(
        cont, &(C4_UI_Text_Config){
            .str = "Amount to Win",
            .font = game->fontRegular,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = PT_SIZE,
            .destinationX = 475.f,
            .destinationY = 450.f,
            .wrapWidth = 0
        }
    );

    settingsData.applyButton = applyButton;
    settingsData.confirmPopup = confirmPopup;
    settingsData.game = game;
    settingsData.heightInput = boardHeightInput;
    settingsData.widthInput = boardWidthInput;
    settingsData.winAmountInput = winAmountInput;
}