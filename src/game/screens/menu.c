#include "Connect4/game/screens/menu.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"

typedef struct {
    C4_Game* game;
    C4_UI_Popup* inDevelopmentPopup;
    C4_UI_Popup* exitGamePopup;
    C4_UI_Text* title;
    C4_UI_ButtonGroup* buttonGroup;
} C4_MenuScreenData;
static C4_MenuScreenData menuData;

void C4_UpdateUILayout_Menu(C4_UI_LayoutType layout) {
    C4_Game* game = menuData.game;
    SDL_Renderer* renderer = game->renderer;

    switch (layout) {
        case C4_UI_LayoutType_Wide: {
            C4_Screen_UpdatePopup(
                menuData.inDevelopmentPopup, 
                &(C4_Screen_UpdatePopup_Config){
                    .buttonGroupHeight = 100.f,
                    .direction = C4_UI_ButtonGroup_Direction_Horizontal,
                    .buttonGroupPtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .messagePtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .destination = (SDL_FRect){0.f, 0.f, 800.f, 400.f},
                    .renderer = renderer,
                }
            );

            C4_Screen_UpdatePopup(
                menuData.exitGamePopup, 
                &(C4_Screen_UpdatePopup_Config){
                    .buttonGroupHeight = 100.f,
                    .direction = C4_UI_ButtonGroup_Direction_Horizontal,
                    .buttonGroupPtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .messagePtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .destination = (SDL_FRect){0.f, 0.f, 800.f, 400.f},
                    .renderer = renderer,
                }
            );

            C4_UI_ButtonGroup_ChangeDestination(
                menuData.buttonGroup,
                (SDL_FRect){0.f, game->windowHeight / 3.25f, 700.f, 500.f}
            );
            C4_UI_ButtonGroup_ChangePtSize(menuData.buttonGroup, 32.f, renderer);
        }; break;
        case C4_UI_LayoutType_Tall: {
            C4_Screen_UpdatePopup(
                menuData.inDevelopmentPopup, 
                &(C4_Screen_UpdatePopup_Config){
                    .buttonGroupHeight = 150.f,
                    .direction = C4_UI_ButtonGroup_Direction_Horizontal,
                    .buttonGroupPtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .messagePtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .destination = (SDL_FRect){0.f, 0.f, 900.f, 500.f},
                    .renderer = renderer,
                }
            );

            C4_Screen_UpdatePopup(
                menuData.exitGamePopup, 
                &(C4_Screen_UpdatePopup_Config){
                    .buttonGroupHeight = 320.f,
                    .direction = C4_UI_ButtonGroup_Direction_Vertical,
                    .buttonGroupPtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .messagePtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .destination = (SDL_FRect){0.f, 0.f, 900.f, 600.f},
                    .renderer = renderer,
                }
            );

            C4_UI_ButtonGroup_ChangeDestination(
                menuData.buttonGroup,
                (SDL_FRect){0.f, game->windowHeight / 3.25f, 800.f, 660.f}
            );
            C4_UI_ButtonGroup_ChangePtSize(menuData.buttonGroup, C4_UI_THEME_DEFAULT.defaultPtSize, renderer);
        }; break;
        default: break;
    }

    // Functions that are the same between both layouts
    C4_UI_Popup_CenterInWindow(
        menuData.inDevelopmentPopup, renderer,
        game->windowWidth, game->windowHeight,
        game->UIScale
    );

    C4_UI_Popup_CenterInWindow(
        menuData.exitGamePopup, renderer,
        game->windowWidth, game->windowHeight,
        game->UIScale
    );

    C4_UI_ButtonGroup_CenterInWindow(
        menuData.buttonGroup, C4_Axis_X, game->windowWidth,
        game->windowHeight, game->UIScale
    );

    C4_UI_CenterInWindow(
        &menuData.title->destination, C4_Axis_X,
        game->windowWidth, game->windowHeight,
        game->UIScale
    );
}

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

    menuData.game = game;

    C4_UI_Canvas* cont = &game->canvas;
    C4_UI_Canvas_Clear(cont);
    game->currentScreen = C4_ScreenType_Menu;

    menuData.inDevelopmentPopup = C4_UI_Canvas_Add_Popup(
        cont, &(C4_UI_Popup_Config){
            .destination = C4_EMPTY_SDL_FRECT,
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .buttonCount = 1,
            .buttonGroupHeight = 100.f,
            .message = "This is still in development!",
            .messageFont = game->fontRegular,
            .buttonFont = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    C4_UI_Button* popupOkButton = &menuData.inDevelopmentPopup->buttonGroup.buttons[0];
    C4_UI_Text_UpdateStr(&popupOkButton->text, "Ok", game->renderer);
    popupOkButton->OnReleaseCallback = OnePlayerPopupOkOnClick;
    popupOkButton->OnClickContext = menuData.inDevelopmentPopup;

    menuData.exitGamePopup = C4_UI_Canvas_Add_Popup(
        cont, &(C4_UI_Popup_Config){
            .destination = C4_EMPTY_SDL_FRECT,
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .buttonCount = 2,
            .buttonGroupHeight = 100.f,
            .message = "Do you want to exit Connect4?",
            .messageFont = game->fontRegular,
            .buttonFont = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    POPUP_CLICK_CONTEXTS[1] = menuData.exitGamePopup;
    BUTTON_GROUP_CLICK_CONTEXTS[0] = menuData.inDevelopmentPopup;
    BUTTON_GROUP_CLICK_CONTEXTS[3] = menuData.exitGamePopup;
    for (size_t i = 0; i < POPUP_BUTTON_COUNT; i++) {
        C4_UI_Button* btn = &menuData.exitGamePopup->buttonGroup.buttons[i];
        C4_UI_Text_UpdateStr(&btn->text, POPUP_TEXT[i], game->renderer);
        btn->OnReleaseCallback = POPUP_ON_CLICKS[i];
        btn->OnClickContext = POPUP_CLICK_CONTEXTS[i];
    }

    menuData.title = C4_UI_Canvas_Add_Text(
        cont, &(C4_UI_Text_Config){
            .str = "Connect4",
            .font = game->fontBold,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = 200.f,
            .destinationX = 0.f,
            .destinationY = 0.f,
            .wrapWidth = 0
        }
    );

    menuData.buttonGroup = C4_UI_Canvas_Add_ButtonGroup(
        cont, &(C4_UI_ButtonGroup_Config){
            .destination = C4_EMPTY_SDL_FRECT,
            .count = BUTTON_GROUP_COUNT,
            .buttonDirection = C4_UI_ButtonGroup_Direction_Vertical,
            .margin = 15,
            .font = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    for (size_t i = 0; i < BUTTON_GROUP_COUNT; i++) {
        C4_UI_Button* btn = &menuData.buttonGroup->buttons[i];
        C4_UI_Text_UpdateStr(&btn->text, MENU_BUTTON_GROUP_TEXT[i], game->renderer);
        btn->OnReleaseCallback = BUTTON_GROUP_ON_CLICKS[i];
        btn->OnClickContext = BUTTON_GROUP_CLICK_CONTEXTS[i];
    }

    C4_UpdateUILayout_Menu(game->currentLayout);
}