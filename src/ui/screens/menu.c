#include "Connect4/ui/screens/menu.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include <stdlib.h>

#define C4_SCREEN_MENU_BUTTONCOUNT 4

typedef enum {
    C4_Screen_Menu_ButtonTextIndexes_1Player,
    C4_Screen_Menu_ButtonTextIndexes_2Players,
    C4_Screen_Menu_ButtonTextIndexes_Settings,
    C4_Screen_Menu_ButtonTextIndexes_Quit
} C4_Screen_Menu_ButtonTextIndexes;
static const char buttonText[C4_SCREEN_MENU_BUTTONCOUNT][16] = {
    "1 Player",
    "2 Players",
    "Settings",
    "Quit"
};

#define C4_SCREEN_MENU_POPUPBUTTONCOUNT 2
typedef enum {
    C4_PopupButtonIndexes_Ok,
    C4_PopupButtonIndexes_Cancel
} C4_PopupButtonIndexes;
static const char popupButtonText[C4_SCREEN_MENU_POPUPBUTTONCOUNT][8] = {
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
            &screen->buttonGroup, screen->renderer, (SDL_FRect){0.f, 375.f, C4_SCREEN_MENU_BUTTON_GROUP_WIDTH, C4_SCREEN_MENU_BUTTON_GROUP_HEIGHT}, C4_SCREEN_MENU_BUTTONCOUNT,
            C4_UI_ButtonGroup_Direction_Vertical, C4_SCREEN_MENU_BUTTON_GROUP_MARGIN, &C4_UI_THEME_DEFAULT
        )
    ) {
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < C4_SCREEN_MENU_BUTTONCOUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(&screen->buttonGroup, i, screen->renderer, buttonText[i], C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT);
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
    for (size_t i = 0; i < C4_SCREEN_MENU_POPUPBUTTONCOUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(
            &screen->exitGamePopup.buttonGroup, i, screen->renderer, popupButtonText[i], 
            C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT
        );
    }
    C4_UI_Popup_CenterInWindow(&screen->exitGamePopup);
    return screen;
}

void C4_Screen_Menu_Destroy(void* screenData) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
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
}

void C4_Screen_Menu_HandleKeyboardInput(void* screenData, SDL_Scancode scancode) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    if (scancode == SDL_SCANCODE_ESCAPE) {
        screen->exitGamePopup.isShowing = !screen->exitGamePopup.isShowing;
    }
    return;
}

void C4_Screen_Menu_HandleMouseEvents(void* screenData, SDL_Event* event) {
    if (!screenData || !event) {
        SDL_Log("Menu screen and/or event is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    if (screen->exitGamePopup.isShowing) {
        switch (C4_UI_Popup_HandleMouseEvents(&screen->exitGamePopup, event)) {
            case C4_PopupButtonIndexes_Ok: {
                C4_PushEvent_CloseWindow();
            }; break;
            case C4_PopupButtonIndexes_Cancel: {
                screen->exitGamePopup.isShowing = false;
            }; break;
        }
        return;
    }
    switch (C4_UI_ButtonGroup_HandleMouseEvents(&screen->buttonGroup, event, screen->renderer)) {
        case C4_Screen_Menu_ButtonTextIndexes_1Player: {
            C4_PushEvent_ScreenChange(C4_ScreenType_Game);
        }; break;
        case C4_Screen_Menu_ButtonTextIndexes_2Players: {
            C4_PushEvent_ScreenChange(C4_ScreenType_Game);
        }; break;
        case C4_Screen_Menu_ButtonTextIndexes_Settings: {
            C4_PushEvent_ScreenChange(C4_ScreenType_Settings);
        }; break;
        case C4_Screen_Menu_ButtonTextIndexes_Quit: {
            screen->exitGamePopup.isShowing = true;
        }; break;
        default: break;
    }
}