#include "Connect4/game/screens/settings.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include <stdio.h>

typedef struct {
    C4_Game* game;
    C4_UI_Text* title;
    C4_UI_ButtonGroup* buttonGroup;
    C4_UI_NumberInput* widthInput;
    C4_UI_NumberInput* heightInput;
    C4_UI_NumberInput* winAmountInput;
    C4_UI_NumberInput* UIScaleInput;
    C4_UI_Popup* confirmPopup;
    C4_UI_Button* applyButton;
    C4_UI_Text* width;
    C4_UI_Text* height;
    C4_UI_Text* winAmount;
    C4_UI_Text* UIScale;
} C4_SettingsScreenData;
static C4_SettingsScreenData settingsData;

void C4_UpdateUILayout_Settings(C4_UI_LayoutType layout) {
    C4_Game* game = settingsData.game;

    switch (layout) {
        case C4_UI_LayoutType_Wide: {
            settingsData.heightInput->background.destination.x = 1175.f;
            C4_UI_NumberInput_ChangeDestination(settingsData.heightInput, settingsData.heightInput->background.destination);
            settingsData.height->destination.x = 475.f;

            settingsData.widthInput->background.destination.x = 1175.f;
            C4_UI_NumberInput_ChangeDestination(settingsData.widthInput, settingsData.widthInput->background.destination);
            settingsData.width->destination.x = 475.f;

            settingsData.winAmountInput->background.destination.x = 1175.f;
            C4_UI_NumberInput_ChangeDestination(settingsData.winAmountInput, settingsData.winAmountInput->background.destination);
            settingsData.winAmount->destination.x = 475.f;

            settingsData.UIScaleInput->background.destination.x = 1175.f;
            C4_UI_NumberInput_ChangeDestination(settingsData.UIScaleInput, settingsData.UIScaleInput->background.destination);
            settingsData.UIScale->destination.x = 475.f;
            
        }; break;
        case C4_UI_LayoutType_Tall: {
            settingsData.heightInput->background.destination.x = 800.f;
            C4_UI_NumberInput_ChangeDestination(settingsData.heightInput, settingsData.heightInput->background.destination);
            settingsData.height->destination.x = 100.f;

            settingsData.widthInput->background.destination.x = 800.f;
            C4_UI_NumberInput_ChangeDestination(settingsData.widthInput, settingsData.widthInput->background.destination);
            settingsData.width->destination.x = 100.f;

            settingsData.winAmountInput->background.destination.x = 800.f;
            C4_UI_NumberInput_ChangeDestination(settingsData.winAmountInput, settingsData.winAmountInput->background.destination);
            settingsData.winAmount->destination.x = 100.f;

            settingsData.UIScaleInput->background.destination.x = 800.f;
            C4_UI_NumberInput_ChangeDestination(settingsData.UIScaleInput, settingsData.UIScaleInput->background.destination);
            settingsData.UIScale->destination.x = 100.f;
        }; break;
        default: break;
    }

    C4_UI_CenterInWindow(
        &settingsData.title->destination, C4_Axis_X,
        game->windowWidth, game->windowHeight,
        game->UIScale
    );

    settingsData.buttonGroup->bounds.y = game->windowHeight - settingsData.buttonGroup->bounds.h - 50.f;
    C4_UI_ButtonGroup_CenterInWindow(settingsData.buttonGroup, C4_Axis_X,
        game->windowWidth, game->windowHeight,
        game->UIScale
    );

    C4_UI_Popup_CenterInWindow(settingsData.confirmPopup, game->renderer,
        game->windowWidth, game->windowHeight,
        game->UIScale
    );
}

static bool IsUpdatingBoard(void) {
    C4_Board* board = settingsData.game->board;
    return (
        board->width != settingsData.widthInput->currentValue ||
        board->height != settingsData.heightInput->currentValue ||
        board->amountToWin != settingsData.winAmountInput->currentValue
    );
}

static bool IsUpdatingSettings(void) {
    return (
        IsUpdatingBoard() ||
        settingsData.UIScaleInput->currentValue / 100.f != settingsData.game->UIScale
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
    if (!IsUpdatingSettings()) {
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
    if (IsUpdatingBoard()) {
        C4_Board_ChangeSize(data->game->board, (uint8_t)data->widthInput->currentValue, (uint8_t)data->heightInput->currentValue);
        data->game->board->amountToWin = (uint8_t)data->winAmountInput->currentValue;
    }
    data->game->UIScale = settingsData.UIScaleInput->currentValue / 100.f;
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
    settingsData.applyButton->isActive = IsUpdatingSettings();
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

static void UniversalWhilePressedFuncs(void) {
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

static void UIScaleIncrementWhilePressed(void* context) {
    (void)context;
    C4_UI_NumberInput* input = settingsData.UIScaleInput;
    if (input->currentValue < input->max) {
        input->currentValue += 10;
        snprintf(input->numberText.str, sizeof(input->numberText.str), "%d", input->currentValue);
        C4_UI_Text_ReloadTexture(&input->numberText, settingsData.game->renderer);
    }
    UniversalWhilePressedFuncs();
}

static void UIScaleDecrementWhilePressed(void* context) {
    (void)context;
    C4_UI_NumberInput* input = settingsData.UIScaleInput;
    if (input->currentValue > input->min) {
        input->currentValue -= 10;
        snprintf(input->numberText.str, sizeof(input->numberText.str), "%d", input->currentValue);
        C4_UI_Text_ReloadTexture(&input->numberText, settingsData.game->renderer);
    }
    UniversalWhilePressedFuncs();
}

void C4_SetScreen_Settings(C4_Game* game) {
    if (!game) {
        return;
    }
    C4_UI_Container* cont = &game->container;
    C4_UI_Container_Clear(cont);
    game->currentScreen = C4_ScreenType_Settings;
    settingsData.game = game;

    settingsData.title = C4_UI_Container_Add_Text(
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

    settingsData.buttonGroup = C4_UI_Container_Add_ButtonGroup(
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
        C4_UI_Button* btn = &settingsData.buttonGroup->buttons[i];
        C4_UI_Text_UpdateStr(&btn->text, SETTINGS_BUTTONS_TEXT[i], game->renderer);
        btn->OnReleaseCallback = BUTTON_GROUP_ON_CLICKS[i];
    }
    settingsData.applyButton = &settingsData.buttonGroup->buttons[0];
    settingsData.applyButton->isActive = false;

    settingsData.confirmPopup = C4_UI_Container_Add_Popup(
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
        C4_UI_Button* btn = &settingsData.confirmPopup->buttonGroup.buttons[i];
        C4_UI_Text_UpdateStr(&btn->text, POPUP_BUTTON_TEXT[i], game->renderer);
        btn->OnReleaseCallback = POPUP_ON_CLICKS[i];
    }

    settingsData.widthInput = C4_UI_Container_Add_NumberInput(
        cont, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 175.f, 200.f, 100.f},
            .min = 2,
            .max = 10,
            .startingValue = game->board->width,
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    C4_UI_Button* boardWidthIncrement = &settingsData.widthInput->buttonGroup.buttons[0];
    boardWidthIncrement->WhilePressedCallback = WidthIncrementWhilePressed;
    C4_UI_Button* boardWidthDecrement = &settingsData.widthInput->buttonGroup.buttons[1];
    boardWidthDecrement->WhilePressedCallback = WidthDecrementWhilePressed;

    const float PT_SIZE = settingsData.widthInput->numberText.ptSize;

    settingsData.width = C4_UI_Container_Add_Text(
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

    settingsData.heightInput = C4_UI_Container_Add_NumberInput(
        cont, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 300.f, 200.f, 100.f},
            .min = 2,
            .max = 6,
            .startingValue = game->board->height,
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    C4_UI_Button* boardHeightIncrement = &settingsData.heightInput->buttonGroup.buttons[0];
    boardHeightIncrement->WhilePressedCallback = HeightIncrementWhilePressed;
    C4_UI_Button* boardHeightDecrement = &settingsData.heightInput->buttonGroup.buttons[1];
    boardHeightDecrement->WhilePressedCallback = HeightDecrementWhilePressed;

    settingsData.height = C4_UI_Container_Add_Text(
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

    settingsData.winAmountInput = C4_UI_Container_Add_NumberInput(
        cont, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 425.f, 200.f, 100.f},
            .min = 2,
            .max = C4_Board_GetMaxAmountToWin(game->board->width, game->board->height),
            .startingValue = game->board->amountToWin,
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    C4_UI_Button* winAmountIncrement = &settingsData.winAmountInput->buttonGroup.buttons[0];
    winAmountIncrement->WhilePressedCallback = WinAmountIncrementWhilePressed;
    C4_UI_Button* winAmountDecrement = &settingsData.winAmountInput->buttonGroup.buttons[1];
    winAmountDecrement->WhilePressedCallback = WinAmountDecrementWhilePressed;

    settingsData.winAmount = C4_UI_Container_Add_Text(
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

    settingsData.UIScaleInput = C4_UI_Container_Add_NumberInput(
        cont, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 550.f, 200.f, 100.f},
            .min = 20,
            .max = 250,
            .startingValue = (unsigned int)(game->UIScale * 100),
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    C4_UI_Button* UIScaleIncrement = &settingsData.UIScaleInput->buttonGroup.buttons[0];
    UIScaleIncrement->WhilePressedCallback = UIScaleIncrementWhilePressed;
    C4_UI_Button* UIScaleDecrement = &settingsData.UIScaleInput->buttonGroup.buttons[1];
    UIScaleDecrement->WhilePressedCallback = UIScaleDecrementWhilePressed;

    settingsData.UIScale = C4_UI_Container_Add_Text(
        cont, &(C4_UI_Text_Config){
            .str = "UI Scale",
            .font = game->fontRegular,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = PT_SIZE,
            .destinationX = 475.f,
            .destinationY = 575.f,
            .wrapWidth = 0
        }
    );

    C4_UpdateUILayout_Settings(game->currentLayout);
}