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

static void C4_SettingsScreen_OnEnter(C4_UI_Screen* screen) {
    if (!screen || !screen->data) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)screen->data;
    data->confirmPopup->isShowing = false;
}

static void C4_SettingsScreen_HandleWindowResize(C4_UI_Screen* screen, C4_UI_LayoutType layout) {
    if (!screen || !screen->data) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)screen->data;
    C4_Game* game = data->game;

    switch (layout) {
        case C4_UI_LayoutType_Wide: {
            data->heightInput->background.destination.x = 1175.f;
            C4_UI_NumberInput_ChangeDestination(data->heightInput, data->heightInput->background.destination);
            data->height->destination.x = 475.f;

            data->widthInput->background.destination.x = 1175.f;
            C4_UI_NumberInput_ChangeDestination(data->widthInput, data->widthInput->background.destination);
            data->width->destination.x = 475.f;

            data->winAmountInput->background.destination.x = 1175.f;
            C4_UI_NumberInput_ChangeDestination(data->winAmountInput, data->winAmountInput->background.destination);
            data->winAmount->destination.x = 475.f;

            data->UIScaleInput->background.destination.x = 1175.f;
            C4_UI_NumberInput_ChangeDestination(data->UIScaleInput, data->UIScaleInput->background.destination);
            data->UIScale->destination.x = 475.f;
            
        }; break;
        case C4_UI_LayoutType_Tall: {
            data->heightInput->background.destination.x = 800.f;
            C4_UI_NumberInput_ChangeDestination(data->heightInput, data->heightInput->background.destination);
            data->height->destination.x = 100.f;

            data->widthInput->background.destination.x = 800.f;
            C4_UI_NumberInput_ChangeDestination(data->widthInput, data->widthInput->background.destination);
            data->width->destination.x = 100.f;

            data->winAmountInput->background.destination.x = 800.f;
            C4_UI_NumberInput_ChangeDestination(data->winAmountInput, data->winAmountInput->background.destination);
            data->winAmount->destination.x = 100.f;

            data->UIScaleInput->background.destination.x = 800.f;
            C4_UI_NumberInput_ChangeDestination(data->UIScaleInput, data->UIScaleInput->background.destination);
            data->UIScale->destination.x = 100.f;
        }; break;
        default: break;
    }

    C4_UI_CenterInWindow(
        &data->title->destination, C4_Axis_X,
        game->presentationWidth, game->presentationHeight,
        game->UIScale
    );

    data->buttonGroup->bounds.y = game->presentationHeight - data->buttonGroup->bounds.h - 50.f;
    C4_UI_ButtonGroup_CenterInWindow(data->buttonGroup, C4_Axis_X,
        game->presentationWidth, game->presentationHeight,
        game->UIScale
    );

    C4_UI_Popup_CenterInWindow(data->confirmPopup, game->renderer,
        game->presentationWidth, game->presentationHeight,
        game->UIScale
    );
}

static bool C4_SettingsScreen_Init(C4_UI_Screen* screen, C4_Game* game);

C4_UI_Screen* C4_SettingsScreen_Create(C4_Game* game) {
    if (!game) {
        return NULL;
    }
    C4_UI_Screen* screen = C4_Screen_Create(game->renderer);
    if (!screen) {
        return NULL;
    }

    screen->data = calloc(1, sizeof(C4_SettingsScreenData));
    if (!screen->data) {
        screen->Destroy(screen);
        return NULL;
    }

    screen->HandleWindowResize = C4_SettingsScreen_HandleWindowResize;
    screen->OnEnter = C4_SettingsScreen_OnEnter;

    if (!C4_SettingsScreen_Init(screen, game)) {
        screen->Destroy(screen);
        return NULL;
    }

    return screen;
}

static bool IsUpdatingBoard(C4_SettingsScreenData* data) {
    if (!data) {
        return false;
    }
    C4_Board* board = data->game->board;
    if (!board) {
        return false;
    }
    return (
        board->width != data->widthInput->currentValue ||
        board->height != data->heightInput->currentValue ||
        board->amountToWin != data->winAmountInput->currentValue
    );
}
static bool IsUpdatingSettings(C4_SettingsScreenData* data) {
    return (
        IsUpdatingBoard(data) ||
        data->UIScaleInput->currentValue / 100.f != data->game->UIScale
    );
}


static const char CONFIRM_MESSAGE_DEFAULT[] = "Are you sure you want to apply these settings?";
static const char CONFIRM_MESSAGE_BOARD_NOT_EMPTY[] = "Are you sure you want to apply these settings? This will clear the current game.";

static void ApplyButton_OnRelease(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    if (!IsUpdatingSettings(data)) {
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
static void CancelButton_OnRelease(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
}

static void Popup_Yes_OnRelease(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    if (IsUpdatingBoard(data)) {
        C4_Board_ChangeSize(data->game->board, (uint8_t)data->widthInput->currentValue, (uint8_t)data->heightInput->currentValue);
        data->game->board->amountToWin = (uint8_t)data->winAmountInput->currentValue;
    }
    data->game->UIScale = data->UIScaleInput->currentValue / 100.f;
    C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
}

static void Popup_Back_OnRelease(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    data->confirmPopup->isShowing = false;
}

static void ApplyButtonInactiveHandler(C4_SettingsScreenData* data) {
    if (!data) {
        return;
    }
    data->applyButton->isActive = IsUpdatingSettings(data);
}

static void ChangeAmountToWinMax(C4_SettingsScreenData* data) {
    if (!data) {
        return;
    }
    C4_UI_NumberInput_ChangeMax(
        data->winAmountInput,
        C4_Board_GetMaxAmountToWin(
            (uint8_t)data->widthInput->currentValue,
            (uint8_t)data->heightInput->currentValue
        ),
        data->game->renderer
    );
}

static void UniversalWhilePressedFuncs(C4_SettingsScreenData* data) {
    if (!data) {
        return;
    }
    ChangeAmountToWinMax(data);
    ApplyButtonInactiveHandler(data);
    C4_UI_NumberInput_GenericButtonInactiveHandler(data->winAmountInput);
    C4_PlaySound(C4_SoundEffect_ButtonClick);
}

static void WidthIncrement_WhilePressed(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    C4_UI_NumberInput_GenericIncrementCallback(data->widthInput);
    C4_UI_NumberInput_GenericButtonInactiveHandler(data->widthInput);
    UniversalWhilePressedFuncs(data);
}

static void WidthDecrement_WhilePressed(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    C4_UI_NumberInput_GenericDecrementCallback(data->widthInput);
    C4_UI_NumberInput_GenericButtonInactiveHandler(data->widthInput);
    UniversalWhilePressedFuncs(data);
}

static void HeightIncrement_WhilePressed(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    C4_UI_NumberInput_GenericIncrementCallback(data->heightInput);
    C4_UI_NumberInput_GenericButtonInactiveHandler(data->heightInput);
    UniversalWhilePressedFuncs(data);
}

static void HeightDecrement_WhilePressed(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    C4_UI_NumberInput_GenericDecrementCallback(data->heightInput);
    C4_UI_NumberInput_GenericButtonInactiveHandler(data->heightInput);
    UniversalWhilePressedFuncs(data);
}

static void WinAmountIncrement_WhilePressed(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    C4_UI_NumberInput_GenericIncrementCallback(data->winAmountInput);
    UniversalWhilePressedFuncs(data);
}

static void WinAmountDecrement_WhilePressed(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    C4_UI_NumberInput_GenericDecrementCallback(data->winAmountInput);
    UniversalWhilePressedFuncs(data);
}

static void UIScaleIncrement_WhilePressed(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    C4_UI_NumberInput* input = data->UIScaleInput;
    if (input->currentValue < input->max) {
        input->currentValue += 10;
        snprintf(input->numberText.str, sizeof(input->numberText.str), "%d", input->currentValue);
        C4_UI_Text_ReloadTexture(&input->numberText, data->game->renderer);
    }
    C4_UI_NumberInput_GenericButtonInactiveHandler(data->UIScaleInput);
    UniversalWhilePressedFuncs(data);
}

static void UIScaleDecrement_WhilePressed(void* context) {
    if (!context) {
        return;
    }
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)context;
    C4_UI_NumberInput* input = data->UIScaleInput;
    if (input->currentValue > input->min) {
        input->currentValue -= 10;
        snprintf(input->numberText.str, sizeof(input->numberText.str), "%d", input->currentValue);
        C4_UI_Text_ReloadTexture(&input->numberText, data->game->renderer);
    }
    C4_UI_NumberInput_GenericButtonInactiveHandler(data->UIScaleInput);
    UniversalWhilePressedFuncs(data);
}

static bool C4_SettingsScreen_Init(C4_UI_Screen* screen, C4_Game* game) {
    if (!screen || !game || !screen->data || !game->renderer) {
        return false;
    }
    C4_UI_Canvas* canvas = &screen->canvas;
    C4_SettingsScreenData* data = (C4_SettingsScreenData*)screen->data;
    SDL_Renderer* renderer = game->renderer;

    data->game = game;

    data->title = C4_UI_Canvas_Add_Text(
        canvas, &(C4_UI_Text_Config){
            .str = "Settings",
            .font = game->fontBold,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = 100.f,
            .destinationX = 0.f,
            .destinationY = 0.f,
            .wrapWidth = 0
        }
    );
    if (!data->title) {
        return false;
    }

    data->buttonGroup = C4_UI_Canvas_Add_ButtonGroup(
        canvas, &(C4_UI_ButtonGroup_Config){
            .destination = (SDL_FRect){0.f, 900.f, 900.f, 100.f},
            .count = 2,
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .margin = 15,
            .font = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->buttonGroup) {
        return false;
    }

    data->applyButton = &data->buttonGroup->buttons[0];
    C4_UI_Text_UpdateStr(&data->applyButton->text, "Apply", renderer);
    data->applyButton->OnReleaseCallback = ApplyButton_OnRelease;
    data->applyButton->OnReleaseContext = data;
    data->applyButton->isActive = false;

    C4_UI_Button* ButtonGroup_CancelButton = &data->buttonGroup->buttons[1];
    C4_UI_Text_UpdateStr(&ButtonGroup_CancelButton->text, "Cancel", renderer);
    ButtonGroup_CancelButton->OnReleaseCallback = CancelButton_OnRelease;
    ButtonGroup_CancelButton->OnReleaseContext = data;

    data->confirmPopup = C4_UI_Canvas_Add_Popup(
        canvas, &(C4_UI_Popup_Config){
            .destination = (SDL_FRect){0.f, 0.f, 800.f, 400.f},
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .buttonCount = 2,
            .buttonGroupHeight = 100.f,
            .message = CONFIRM_MESSAGE_DEFAULT,
            .messageFont = game->fontRegular,
            .buttonFont = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->confirmPopup) {
        return false;
    }

    C4_UI_Button* Popup_Yes = &data->confirmPopup->buttonGroup.buttons[0];
    C4_UI_Text_UpdateStr(&Popup_Yes->text, "Yes", renderer);
    Popup_Yes->OnReleaseCallback = Popup_Yes_OnRelease;
    Popup_Yes->OnReleaseContext = data;

    C4_UI_Button* Popup_Back = &data->confirmPopup->buttonGroup.buttons[1];
    C4_UI_Text_UpdateStr(&Popup_Back->text, "Back", renderer);
    Popup_Back->OnReleaseCallback = Popup_Back_OnRelease;
    Popup_Back->OnReleaseContext = data;

    data->widthInput = C4_UI_Canvas_Add_NumberInput(
        canvas, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 175.f, 200.f, 100.f},
            .min = 2,
            .max = 10,
            .startingValue = game->board->width,
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->widthInput) {
        return false;
    }

    C4_UI_Button* boardWidthIncrement = &data->widthInput->buttonGroup.buttons[0];
    boardWidthIncrement->WhilePressedCallback = WidthIncrement_WhilePressed;
    boardWidthIncrement->WhilePressedContext = data;

    C4_UI_Button* boardWidthDecrement = &data->widthInput->buttonGroup.buttons[1];
    boardWidthDecrement->WhilePressedCallback = WidthDecrement_WhilePressed;
    boardWidthDecrement->WhilePressedContext = data;

    const float PT_SIZE = data->widthInput->numberText.ptSize;

    data->width = C4_UI_Canvas_Add_Text(
        canvas, &(C4_UI_Text_Config){
            .str = "Board Width",
            .font = game->fontRegular,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = PT_SIZE,
            .destinationX = 475.f,
            .destinationY = 200.f,
            .wrapWidth = 0
        }
    );
    if (!data->width) {
        return false;
    }

    data->heightInput = C4_UI_Canvas_Add_NumberInput(
        canvas, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 300.f, 200.f, 100.f},
            .min = 2,
            .max = 6,
            .startingValue = game->board->height,
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->heightInput) {
        return false;
    }

    C4_UI_Button* boardHeightIncrement = &data->heightInput->buttonGroup.buttons[0];
    boardHeightIncrement->WhilePressedCallback = HeightIncrement_WhilePressed;
    boardHeightIncrement->WhilePressedContext = data;

    C4_UI_Button* boardHeightDecrement = &data->heightInput->buttonGroup.buttons[1];
    boardHeightDecrement->WhilePressedCallback = HeightDecrement_WhilePressed;
    boardHeightDecrement->WhilePressedContext = data;


    data->height = C4_UI_Canvas_Add_Text(
        canvas, &(C4_UI_Text_Config){
            .str = "Board Height",
            .font = game->fontRegular,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = PT_SIZE,
            .destinationX = 475.f,
            .destinationY = 325.f,
            .wrapWidth = 0
        }
    );
    if (!data->height) {
        return false;
    }

    data->winAmountInput = C4_UI_Canvas_Add_NumberInput(
        canvas, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 425.f, 200.f, 100.f},
            .min = 2,
            .max = C4_Board_GetMaxAmountToWin(game->board->width, game->board->height),
            .startingValue = game->board->amountToWin,
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->winAmountInput) {
        return false;
    }

    C4_UI_Button* winAmountIncrement = &data->winAmountInput->buttonGroup.buttons[0];
    winAmountIncrement->WhilePressedCallback = WinAmountIncrement_WhilePressed;
    winAmountIncrement->WhilePressedContext = data;

    C4_UI_Button* winAmountDecrement = &data->winAmountInput->buttonGroup.buttons[1];
    winAmountDecrement->WhilePressedCallback = WinAmountDecrement_WhilePressed;
    winAmountDecrement->WhilePressedContext = data;

    data->winAmount = C4_UI_Canvas_Add_Text(
        canvas, &(C4_UI_Text_Config){
            .str = "Amount to Win",
            .font = game->fontRegular,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = PT_SIZE,
            .destinationX = 475.f,
            .destinationY = 450.f,
            .wrapWidth = 0
        }
    );
    if (!data->winAmount) {
        return false;
    }

    data->UIScaleInput = C4_UI_Canvas_Add_NumberInput(
        canvas, &(C4_UI_NumberInput_Config){
            .destination = (SDL_FRect){1175.f, 550.f, 200.f, 100.f},
            .min = 20,
            .max = 250,
            .startingValue = (unsigned int)(game->UIScale * 100),
            .font = game->fontRegular,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->UIScaleInput) {
        return false;
    }

    C4_UI_Button* UIScaleIncrement = &data->UIScaleInput->buttonGroup.buttons[0];
    UIScaleIncrement->WhilePressedCallback = UIScaleIncrement_WhilePressed;
    UIScaleIncrement->WhilePressedContext = data;

    C4_UI_Button* UIScaleDecrement = &data->UIScaleInput->buttonGroup.buttons[1];
    UIScaleDecrement->WhilePressedCallback = UIScaleDecrement_WhilePressed;
    UIScaleDecrement->WhilePressedContext = data;

    data->UIScale = C4_UI_Canvas_Add_Text(
        canvas, &(C4_UI_Text_Config){
            .str = "UI Scale",
            .font = game->fontRegular,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = PT_SIZE,
            .destinationX = 475.f,
            .destinationY = 575.f,
            .wrapWidth = 0
        }
    );
    if (!data->UIScale) {
        return false;
    }

    screen->HandleWindowResize(screen, game->currentLayout);

    return true;
}