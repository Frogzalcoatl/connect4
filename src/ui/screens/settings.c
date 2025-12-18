#include "Connect4/ui/screens/settings.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include <stdlib.h>

#define SETTINGS_BUTTON_COUNT 2
typedef enum {
    Settings_ButtonGroup_Apply,
    Settings_ButtonGroup_Cancel
} Settings_ButtonGroup;
static const char SETTINGS_BUTTONS_TEXT[SETTINGS_BUTTON_COUNT][16] = {
    "Apply",
    "Cancel",
};

#define SETTINGS_POPUP_BUTTONS_COUNT 2
typedef enum {
    Settings_PopupButtons_Yes,
    Settings_PopupButtons_Back
} Settings_PopupButtons;
static const char SETTINGS_POPUP_BUTTONS_TEXT[SETTINGS_POPUP_BUTTONS_COUNT][8] = {
    "Yes",
    "Back"
};

C4_Screen_Settings* C4_Screen_Settings_Create(SDL_Renderer* renderer, SDL_Window* window) {
    if (!window || !renderer) {
        SDL_Log("Unable to create settings screen. Window and/or renderer is NULL");
        return NULL;
    }
    C4_Screen_Settings* screen = calloc(1, sizeof(C4_Screen_Settings));
    if (!screen) {
        SDL_Log("Unable to allocate memory for settings screen");
        return NULL;
    }
    screen->window = window;
    screen->renderer = renderer;
    if (!C4_UI_Text_InitProperties(&screen->title, screen->renderer, "Settings", C4_FontType_Bold, C4_SCREEN_SETTINGS_TITLE_PT_SIZE, 0.f, 0.f, 0, C4_UI_THEME_DEFAULT.textColor)) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    C4_UI_CenterInWindow(&screen->title.destination, C4_Axis_X);
    if (
        !C4_UI_ButtonGroup_InitProperties(
            &screen->buttonGroup, renderer,
            (SDL_FRect){0.f, C4_SCREEN_SETTINGS_BUTTON_GROUP_YPOS, C4_SCREEN_SETTINGS_BUTTON_GROUP_WIDTH, C4_SCREEN_SETTINGS_BUTTON_GROUP_HEIGHT},
            SETTINGS_BUTTON_COUNT, C4_UI_ButtonGroup_Direction_Horizontal, 15, &C4_UI_THEME_DEFAULT
        )
    ) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < SETTINGS_BUTTON_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(&screen->buttonGroup, i, screen->renderer, SETTINGS_BUTTONS_TEXT[i], C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT);
    }
    C4_UI_ButtonGroup_CenterInWindow(&screen->buttonGroup, C4_Axis_X);
    if (!C4_UI_Popup_InitProperties(
        &screen->confirmationPopup, screen->renderer,
        (SDL_FRect){0.f, 0.f, C4_SCREEN_SETTINGS_POPUP_WIDTH, C4_SCREEN_SETTINGS_POPUP_HEIGHT},
        C4_UI_ButtonGroup_Direction_Horizontal, 2, 100.f,
        "Are you sure you want to apply these settings?", &C4_UI_THEME_DEFAULT
    )) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < SETTINGS_POPUP_BUTTONS_COUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(&screen->confirmationPopup.buttonGroup, i, screen->renderer, SETTINGS_POPUP_BUTTONS_TEXT[i], C4_UI_SymbolType_None, 0.f, 0.f, 0, &C4_UI_THEME_DEFAULT);
    }
    C4_UI_Popup_CenterInWindow(&screen->confirmationPopup);
    return screen;
}

void C4_Screen_Settings_Destroy(void* screenData) {
    if (!screenData) {
        SDL_Log("Settings screen is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    C4_UI_ButtonGroup_FreeResources(&screen->buttonGroup);
    C4_UI_Popup_FreeResources(&screen->confirmationPopup);
    C4_UI_Text_FreeResources(&screen->title);
    free(screen);
}

void C4_Screen_Settings_Draw(void* screenData) {
    if (!screenData) {
        SDL_Log("Settings screen is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    C4_UI_ButtonGroup_Draw(&screen->buttonGroup, screen->renderer);
    C4_UI_Text_Draw(&screen->title, screen->renderer);
    C4_UI_Popup_Draw(&screen->confirmationPopup);
}

void C4_Screen_Settings_HandleKeyboardInput(void* screenData, SDL_Scancode scancode) {
    if (!screenData) {
        SDL_Log("Settings screen is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    if (scancode == SDL_SCANCODE_ESCAPE) {
        if (screen->confirmationPopup.isShowing) {
            screen->confirmationPopup.isShowing = false;
        } else {
            C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
        }
    }
}

void C4_Screen_Settings_HandleMouseEvents(void* screenData, SDL_Event* event) {
    if (!screenData || !event) {
        SDL_Log("Settings screen and/or event is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    switch (C4_UI_Popup_HandleMouseEvents(&screen->confirmationPopup, event)) {
        case Settings_PopupButtons_Yes: {  
            screen->confirmationPopup.isShowing = false;
            C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
        }; return;
        case Settings_PopupButtons_Back: {
            screen->confirmationPopup.isShowing = false;
        }; return;
        default: break;
    }
    if (screen->confirmationPopup.isShowing) {
        return;
    }
    switch (C4_UI_ButtonGroup_HandleMouseEvents(&screen->buttonGroup, event, screen->renderer)) {
        case Settings_ButtonGroup_Apply: {
            screen->confirmationPopup.isShowing = true;
            screen->buttonGroup.buttons[0].background.color = screen->buttonGroup.buttons[0].defaultColors.background;
        }; break;
        case Settings_ButtonGroup_Cancel: {
            C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
        }; break;
        default: break;
    }
}