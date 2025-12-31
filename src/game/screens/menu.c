#include "Connect4/game/screens/menu.h"
#include "Connect4/constants.h"

#define MENU_BUTTON_GROUP_COUNT 4
static const char MENU_BUTTON_GROUP_TEXT[MENU_BUTTON_GROUP_COUNT][16] = {
    "1 Player",
    "2 Players",
    "Settings",
    "Quit"
};

#define MENU_POPUP_BUTTONS_COUNT 2
static const char MENU_POPUP_BUTTONS_TEXT[MENU_POPUP_BUTTONS_COUNT][8] = {
    "Ok",
    "Cancel"
};

void C4_SetScreen_Menu(C4_Game* game) {
    if (game == NULL) {
        return;
    }
    C4_UI_Container* cont = &game->container;
    C4_UI_Container_Clear(cont);
    game->currentScreen = C4_ScreenType_Menu;
    C4_UI_Text* title = C4_UI_Container_Add_Text(
        cont, "Connect4", C4_FontType_Bold, C4_SCREEN_MENU_TITLE_PT_SIZE,
        0.f, 50.f, 0, C4_UI_THEME_DEFAULT.textColor
    );
    C4_UI_CenterInWindow(&title->destination, C4_Axis_X);
    C4_UI_ButtonGroup* buttonGroup = C4_UI_Container_Add_ButtonGroup(
        cont, (SDL_FRect){0.f, 375.f, C4_SCREEN_MENU_BUTTON_GROUP_WIDTH, C4_SCREEN_MENU_BUTTON_GROUP_HEIGHT},
        MENU_BUTTON_GROUP_COUNT, C4_UI_ButtonGroup_Direction_Vertical, C4_SCREEN_MENU_BUTTON_GROUP_MARGIN,
        &C4_UI_THEME_DEFAULT
    );
    for (size_t i = 0; i < MENU_BUTTON_GROUP_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            buttonGroup, i, game->renderer, MENU_BUTTON_GROUP_TEXT[i], C4_UI_SymbolType_None,
            0.f, 0.f, 0, &C4_UI_THEME_DEFAULT
        );
    }
    C4_UI_ButtonGroup_CenterInWindow(buttonGroup, C4_Axis_X);
    C4_UI_Popup* exitGamePopup = C4_UI_Container_Add_Popup(
        cont, (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH, C4_SCREEN_SETTINGS_POPUP_HEIGHT},
        C4_UI_ButtonGroup_Direction_Horizontal, 2, 100.f, "Do you want to exit Connect4?", &C4_UI_THEME_DEFAULT
    );
    for (size_t i = 0; i < MENU_POPUP_BUTTONS_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            &exitGamePopup->buttonGroup, i, game->renderer, MENU_POPUP_BUTTONS_TEXT[i], 
            C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT
        );
    }
    C4_UI_Popup_CenterInWindow(exitGamePopup, game->renderer);
}