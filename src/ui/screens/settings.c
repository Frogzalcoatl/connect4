#include "Connect4/ui/screens/settings.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/game/events.h"
#include <stdlib.h>

#define C4_SCREEN_SETTINGS_BUTTONCOUNT 2
typedef enum {
    C4_ButtonGroupIndexes_Apply,
    C4_ButtonGroupIndexes_Cancel
} C4_ButtonGroupIndexes;
static const char buttonText[C4_SCREEN_SETTINGS_BUTTONCOUNT][16] = {
    "Apply",
    "Cancel",
};

#define C4_SCREEN_SETTINGS_POPUPBUTTONCOUNT 2
typedef enum {
    C4_PopupButtonIndexes_Yes,
    C4_PopupButtonIndexes_Back
} C4_PopupButtonIndexes;
static const char popupButtonText[C4_SCREEN_SETTINGS_POPUPBUTTONCOUNT][8] = {
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
    if (!C4_UI_Text_InitProperties(&screen->title, screen->renderer, "Settings", C4_FontType_Bold, 100.f, 0.f, 0.f, 0)) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    C4_UI_CenterInWindow(&screen->title.destination, C4_Axis_X);
    if (
        !C4_UI_ButtonGroup_InitProperties(
            &screen->buttonGroup, renderer, (SDL_FRect){0.f, 900.f, 900.f, 100.f},
            C4_SCREEN_SETTINGS_BUTTONCOUNT, C4_UI_ButtonGroup_Direction_Horizontal, 15, 32.f
        )
    ) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < C4_SCREEN_SETTINGS_BUTTONCOUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(&screen->buttonGroup, i, screen->renderer, buttonText[i], C4_FontType_Bold, 32.f);
    }
    C4_UI_ButtonGroup_CenterInWindow(&screen->buttonGroup, C4_Axis_X);
    if (!C4_UI_Popup_InitProperties(
        &screen->confirmationPopup, screen->renderer, (SDL_FRect){0, 0, 800, 400}, 3, C4_UI_ButtonGroup_Direction_Horizontal,
        2, 100.f, "Are you sure you want to apply these settings?", 32.f, 32.f
    )) {
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < C4_SCREEN_SETTINGS_POPUPBUTTONCOUNT; i++) {
        C4_UI_ButtonGroup_SetButtonIndex(&screen->confirmationPopup.buttonGroup, i, screen->renderer, popupButtonText[i], C4_FontType_Bold, 32.f);
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
}

void C4_Screen_Settings_HandleMouseEvents(void* screenData, SDL_Event* event) {
    if (!screenData || !event) {
        SDL_Log("Settings screen and/or event is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    switch (C4_UI_Popup_HandleMouseEvents(&screen->confirmationPopup, event)) {
        case C4_PopupButtonIndexes_Yes: {  
            screen->confirmationPopup.isShowing = false;
            C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
        }; return;
        case C4_PopupButtonIndexes_Back: {
            screen->confirmationPopup.isShowing = false;
        }; return;
        default: break;
    }
    if (screen->confirmationPopup.isShowing) {
        return;
    }
    switch (C4_UI_ButtonGroup_HandleMouseEvents(&screen->buttonGroup, event, screen->renderer)) {
        case C4_ButtonGroupIndexes_Apply: {
            screen->confirmationPopup.isShowing = true;
            screen->buttonGroup.buttons[0].background.color = screen->buttonGroup.buttons[0].defaultColors.background;
        }; break;
        case C4_ButtonGroupIndexes_Cancel: {
            C4_PushEvent_ScreenChange(C4_ScreenType_Menu);
        }; break;
        default: break;
    }
}