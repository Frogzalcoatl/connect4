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
    if (game == NULL) {
        return;
    }
    C4_UI_Container* cont = &game->container;
    C4_UI_Container_Clear(cont);
    game->currentScreen = C4_ScreenType_Menu;

    C4_UI_Popup* exitGamePopup = C4_UI_Container_Add_Popup(
        cont, (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH, C4_SCREEN_SETTINGS_POPUP_HEIGHT},
        C4_UI_ButtonGroup_Direction_Horizontal, 2, 100.f, "Do you want to exit Connect4?", &C4_UI_THEME_DEFAULT
    );

    C4_UI_Popup* inDevelopmentPopup = C4_UI_Container_Add_Popup(
        cont, (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH, C4_SCREEN_SETTINGS_POPUP_HEIGHT},
        C4_UI_ButtonGroup_Direction_Horizontal, 1, 100.f, "This is still in development!", &C4_UI_THEME_DEFAULT
    );
    C4_UI_ButtonGroup_SetButtonIndex(
        &inDevelopmentPopup->buttonGroup, 0, game->renderer, "Ok", C4_UI_SymbolType_None,
        0.f, 0.f, 0, &C4_UI_THEME_DEFAULT,
        OnePlayerPopupOkOnClick, inDevelopmentPopup
    );
    C4_UI_Popup_CenterInWindow(inDevelopmentPopup, game->renderer);

    POPUP_CLICK_CONTEXTS[1] = exitGamePopup;
    BUTTON_GROUP_CLICK_CONTEXTS[0] = inDevelopmentPopup;
    BUTTON_GROUP_CLICK_CONTEXTS[3] = exitGamePopup;

    for (size_t i = 0; i < POPUP_BUTTON_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            &exitGamePopup->buttonGroup, i, game->renderer, POPUP_TEXT[i], 
            C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT,
            POPUP_ON_CLICKS[i], POPUP_CLICK_CONTEXTS[i]
        );
    }
    C4_UI_Popup_CenterInWindow(exitGamePopup, game->renderer);

    C4_UI_Text* title = C4_UI_Container_Add_Text(
        cont, "Connect4", C4_FontType_Bold, C4_SCREEN_MENU_TITLE_PT_SIZE,
        0.f, 50.f, 0, C4_UI_THEME_DEFAULT.textColor
    );
    C4_UI_CenterInWindow(&title->destination, C4_Axis_X);

    C4_UI_ButtonGroup* buttonGroup = C4_UI_Container_Add_ButtonGroup(
        cont, (SDL_FRect){0.f, 375.f, C4_SCREEN_MENU_BUTTON_GROUP_WIDTH, C4_SCREEN_MENU_BUTTON_GROUP_HEIGHT},
        BUTTON_GROUP_COUNT, C4_UI_ButtonGroup_Direction_Vertical, C4_SCREEN_MENU_BUTTON_GROUP_MARGIN,
        &C4_UI_THEME_DEFAULT
    );
    for (size_t i = 0; i < BUTTON_GROUP_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            buttonGroup, i, game->renderer, MENU_BUTTON_GROUP_TEXT[i],
            C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT,
            BUTTON_GROUP_ON_CLICKS[i], BUTTON_GROUP_CLICK_CONTEXTS[i]
        );
    }
    C4_UI_ButtonGroup_CenterInWindow(buttonGroup, C4_Axis_X);
}