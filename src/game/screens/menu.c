#include "Connect4/game/screens/menu.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"

#define BUTTON_GROUP_COUNT 4
static const char MENU_BUTTON_GROUP_TEXT[BUTTON_GROUP_COUNT][16] = {
    "1 Player",
    "2 Players",
    "Settings",
    "Quit"
};
static void OnePlayerOnClick(void* context) {
    C4_UI_Popup* popup = (C4_UI_Popup*)context;
    popup->isShowing = true;
}
static void TwoPlayerOnClick(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Game);
}
static void SettingsOnClick(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Settings);
}
static void QuitOnClick(void* context) {
    C4_UI_Popup* popup = (C4_UI_Popup*)context;
    popup->isShowing = true;
}
static const C4_UI_Callback BUTTON_GROUP_ON_CLICKS[BUTTON_GROUP_COUNT] = {
    OnePlayerOnClick,
    TwoPlayerOnClick,
    SettingsOnClick,
    QuitOnClick
};
static void* BUTTON_GROUP_CLICK_CONTEXTS[BUTTON_GROUP_COUNT] = {0};

#define POPUP_BUTTON_COUNT 2
static const char POPUP_TEXT[POPUP_BUTTON_COUNT][8] = {
    "Ok",
    "Cancel"
};
static void PopupOkOnClick(void* context) {
    (void)context;
    C4_PushEvent_CloseWindow();
}
static void PopupCancelOnClick(void* context) {
    C4_UI_Popup* popup = (C4_UI_Popup*)context;
    popup->isShowing = false;
}
static const C4_UI_Callback POPUP_ON_CLICKS[POPUP_BUTTON_COUNT] = {
    PopupOkOnClick,
    PopupCancelOnClick
};
static void* POPUP_CLICK_CONTEXTS[POPUP_BUTTON_COUNT] = {0};

static void OnePlayerPopupOkOnClick(void* context) {
    C4_UI_Popup* popup = (C4_UI_Popup*)context;
    popup->isShowing = false;
}

void C4_SetScreen_Menu(C4_Game* game) {
    if (!game) {
        return;
    }
    C4_UI_Container* cont = &game->container;
    C4_UI_Container_Clear(cont);
    game->currentScreen = C4_ScreenType_Menu;

    C4_UI_Popup* inDevelopmentPopup = C4_UI_Container_Add_Popup(
        cont, &(C4_UI_Popup_Config){
            .destination = (SDL_FRect){0.f, 0.f, 800.f, 400.f},
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .buttonCount = 1,
            .buttonGroupHeight = 100.f,
            .message = "This is still in development!",
            .messageFont = game->fontRegular,
            .buttonFont = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    C4_UI_Button* popupOkButton = &inDevelopmentPopup->buttonGroup.buttons[0];
    C4_UI_Text_UpdateStr(&popupOkButton->text, "Ok", game->renderer);
    popupOkButton->OnClickCallback = OnePlayerPopupOkOnClick;
    popupOkButton->OnClickContext = inDevelopmentPopup;
    C4_UI_Popup_CenterInWindow(inDevelopmentPopup, game->renderer);

    C4_UI_Popup* exitGamePopup = C4_UI_Container_Add_Popup(
        cont, &(C4_UI_Popup_Config){
            .destination = (SDL_FRect){0.f, 0.f, 800.f, 400.f},
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .buttonCount = 2,
            .buttonGroupHeight = 100.f,
            .message = "Do you want to exit Connect4?",
            .messageFont = game->fontRegular,
            .buttonFont = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );

    POPUP_CLICK_CONTEXTS[1] = exitGamePopup;
    BUTTON_GROUP_CLICK_CONTEXTS[0] = inDevelopmentPopup;
    BUTTON_GROUP_CLICK_CONTEXTS[3] = exitGamePopup;

    for (size_t i = 0; i < POPUP_BUTTON_COUNT; i++) {
        C4_UI_Button* btn = &exitGamePopup->buttonGroup.buttons[i];
        C4_UI_Text_UpdateStr(&btn->text, POPUP_TEXT[i], game->renderer);
        btn->OnClickCallback = POPUP_ON_CLICKS[i];
        btn->OnClickContext = POPUP_CLICK_CONTEXTS[i];
    }
    C4_UI_Popup_CenterInWindow(exitGamePopup, game->renderer);

    C4_UI_Text* title = C4_UI_Container_Add_Text(
        cont, &(C4_UI_Text_Config){
            .str = "Connect4",
            .font = game->fontBold,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = 200.f,
            .destinationX = 0.f,
            .destinationY = 50.f,
            .wrapWidth = 0
        }
    );
    C4_UI_CenterInWindow(&title->destination, C4_Axis_X);

    C4_UI_ButtonGroup* buttonGroup = C4_UI_Container_Add_ButtonGroup(
        cont, &(C4_UI_ButtonGroup_Config){
            .destination = (SDL_FRect){0.f, 375.f, 700.f, 500.f},
            .count = BUTTON_GROUP_COUNT,
            .buttonDirection = C4_UI_ButtonGroup_Direction_Vertical,
            .margin = 15,
            .font = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    for (size_t i = 0; i < BUTTON_GROUP_COUNT; i++) {
        C4_UI_Button* btn = &buttonGroup->buttons[i];
        C4_UI_Text_UpdateStr(&btn->text, MENU_BUTTON_GROUP_TEXT[i], game->renderer);
        btn->OnClickCallback = BUTTON_GROUP_ON_CLICKS[i];
        btn->OnClickContext = BUTTON_GROUP_CLICK_CONTEXTS[i];
    }
    C4_UI_ButtonGroup_CenterInWindow(buttonGroup, C4_Axis_X);
}