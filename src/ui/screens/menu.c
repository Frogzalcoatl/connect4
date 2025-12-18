#include "Connect4/ui/screens/menu.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include <stdlib.h>

#define MENU_BUTTON_GROUP_COUNT 4
typedef enum {
    Menu_ButtonGroup_1Player,
    Menu_ButtonGroup_2Players,
    Menu_ButtonGroup_Settings,
    Menu_ButtonGroup_Quit
} Menu_ButtonGroup;
static const char MENU_BUTTON_GROUP_TEXT[MENU_BUTTON_GROUP_COUNT][16] = {
    "1 Player",
    "2 Players",
    "Settings",
    "Quit"
};

#define MENU_POPUP_BUTTONS_COUNT 2
typedef enum {
    Menu_ExitPopupButtons_Ok,
    Menu_ExitPopupButtons_Cancel
} Menu_ExitPopupButtons;
static const char MENU_POPUP_BUTTONS_TEXT[MENU_POPUP_BUTTONS_COUNT][8] = {
    "Ok",
    "Cancel"
};

C4_Screen_Menu* C4_Screen_Menu_Create(SDL_Renderer* renderer) {
    if (!renderer) {
        SDL_Log("Unable to create menu screen. renderer is NULL");
        return NULL;
    }
    C4_Screen_Menu* screen = calloc(1, sizeof(C4_Screen_Menu));
    if (!screen) {
        SDL_Log("Unable to allocate memory for menu screen");
        return NULL;
    }
    screen->renderer = renderer;
    if (!C4_UI_Text_InitProperties(&screen->title, screen->renderer, "Connect4", C4_FontType_Bold, C4_SCREEN_MENU_TITLE_PT_SIZE, 0.f, 50.f, 0, C4_UI_THEME_DEFAULT.textColor)) {
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    C4_UI_CenterInWindow(&screen->title.destination, C4_Axis_X);
    if (
        !C4_UI_ButtonGroup_InitProperties(
            &screen->buttonGroup, screen->renderer, (SDL_FRect){0.f, 375.f, C4_SCREEN_MENU_BUTTON_GROUP_WIDTH, C4_SCREEN_MENU_BUTTON_GROUP_HEIGHT}, MENU_BUTTON_GROUP_COUNT,
            C4_UI_ButtonGroup_Direction_Vertical, C4_SCREEN_MENU_BUTTON_GROUP_MARGIN, &C4_UI_THEME_DEFAULT
        )
    ) {
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < MENU_BUTTON_GROUP_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(&screen->buttonGroup, i, screen->renderer, MENU_BUTTON_GROUP_TEXT[i], C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT);
    }
    C4_UI_ButtonGroup_CenterInWindow(&screen->buttonGroup, C4_Axis_X);
    if (
        !C4_UI_Popup_InitProperties(
            &screen->exitGamePopup, screen->renderer, (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH, C4_SCREEN_SETTINGS_POPUP_HEIGHT},
            C4_UI_ButtonGroup_Direction_Horizontal, 2, 100.f, "Do you want to exit Connect4?", &C4_UI_THEME_DEFAULT
        )
    ) {
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < MENU_POPUP_BUTTONS_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            &screen->exitGamePopup.buttonGroup, i, screen->renderer, MENU_POPUP_BUTTONS_TEXT[i], 
            C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT
        );
    }
    C4_UI_Popup_CenterInWindow(&screen->exitGamePopup);
    if (
        !C4_UI_Popup_InitProperties(
            &screen->inDevelopmentPopup, screen->renderer, (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH, C4_SCREEN_SETTINGS_POPUP_HEIGHT},
            C4_UI_ButtonGroup_Direction_Horizontal, 1, 100.f, "This is still in development!", &C4_UI_THEME_DEFAULT
        )
    ) {
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    C4_UI_ButtonGroup_SetButtonIndex(
        &screen->inDevelopmentPopup.buttonGroup, 0, screen->renderer, "Ok", C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT
    );
    C4_UI_Popup_CenterInWindow(&screen->inDevelopmentPopup);
    return screen;
}

void C4_Screen_Menu_Destroy(void* screenData) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    C4_UI_Popup_FreeResources(&screen->inDevelopmentPopup);
    C4_UI_Popup_FreeResources(&screen->exitGamePopup);
    C4_UI_ButtonGroup_FreeResources(&screen->buttonGroup);
    C4_UI_Text_FreeResources(&screen->title);
    free(screen);
}

void C4_Screen_Menu_Draw(void* screenData) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    C4_UI_ButtonGroup_Draw(&screen->buttonGroup, screen->renderer);
    C4_UI_Text_Draw(&screen->title, screen->renderer);
    C4_UI_Popup_Draw(&screen->exitGamePopup);
    C4_UI_Popup_Draw(&screen->inDevelopmentPopup);
}

void C4_Screen_Menu_HandleKeyboardInput(void* screenData, SDL_Scancode scancode) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    if (scancode == SDL_SCANCODE_ESCAPE) {
        if (screen->inDevelopmentPopup.isShowing) {
            screen->inDevelopmentPopup.isShowing = false;
            return;
        }
        screen->exitGamePopup.isShowing = !screen->exitGamePopup.isShowing;
    }
}

void C4_Screen_Menu_HandleMouseEvents(void* screenData, SDL_Event* event) {
    if (!screenData || !event) {
        SDL_Log("Menu screen and/or event is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    if (screen->exitGamePopup.isShowing) {
        switch (C4_UI_Popup_HandleMouseEvents(&screen->exitGamePopup, event)) {
            case Menu_ExitPopupButtons_Ok: {
                C4_PushEvent_CloseWindow();
            }; break;
            case Menu_ExitPopupButtons_Cancel: {
                screen->exitGamePopup.isShowing = false;
            }; break;
        }
        return;
    }
    if (screen->inDevelopmentPopup.isShowing) {
        // Only one button on this popup
        if (C4_UI_Popup_HandleMouseEvents(&screen->inDevelopmentPopup, event) == 0) {
            screen->inDevelopmentPopup.isShowing = false;
        }
        return;
    }
    switch (C4_UI_ButtonGroup_HandleMouseEvents(&screen->buttonGroup, event, screen->renderer)) {
        case Menu_ButtonGroup_1Player: {
            screen->inDevelopmentPopup.isShowing = true;
        }; break;
        case Menu_ButtonGroup_2Players: {
            C4_PushEvent_ScreenChange(C4_ScreenType_Game);
        }; break;
        case Menu_ButtonGroup_Settings: {
            C4_PushEvent_ScreenChange(C4_ScreenType_Settings);
        }; break;
        case Menu_ButtonGroup_Quit: {
            screen->exitGamePopup.isShowing = true;
        }; break;
        default: break;
    }
}