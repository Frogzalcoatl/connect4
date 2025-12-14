#include "Connect4/ui/screens/settings.h"
#include "Connect4/assets/fonts.h"
#include <stdlib.h>

C4_Screen_Settings* C4_SettingsScreen_Create(SDL_Renderer* renderer, SDL_Window* window) {
    if (!window || !renderer) {
        return NULL;
    }
    C4_Screen_Settings* screen = calloc(1, sizeof(C4_Screen_Settings));
    if (!screen) {
        return NULL;
    }
    screen->window = window;
    screen->renderer = renderer;
    screen->title = C4_TextUIElement_Create(screen->renderer, "Settings", C4_FontType_Bold, 100.f, (SDL_Color){255, 255, 255, 255}, 0.f, 0.f);
    if (!screen->title) {
        C4_SettingsScreen_Destroy(screen);
        return NULL;
    }
    C4_TextUIElement_CenterInWindow(screen->title, C4_Axis_X);
    screen->backButton = C4_Button_Create(
        screen->renderer, "Back", C4_FontType_Bold, 32.f,
        (SDL_FRect){0.f, 800.f, 400.f, 100.f },
        (C4_UI_ButtonColorInfo){(SDL_Color){150, 150, 150, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_UI_ButtonColorInfo){(SDL_Color){200, 200, 200, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_UI_ButtonColorInfo){(SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255}},
        C4_ScreenChangeRequest_Menu
    );
    C4_Button_CenterInWindow(screen->backButton, C4_Axis_X);
    return screen;
}

void C4_SettingsScreen_Destroy(void* screenData) {
    if (!screenData) {
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    C4_TextUIElement_Destroy(screen->title);
    free(screen);
}

void C4_SettingsScreen_Draw(void* screenData) {
    if (!screenData) {
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    C4_Button_Draw(screen->backButton, screen->renderer);
    C4_TextUIElement_Draw(screen->title, screen->renderer);
}

C4_Screen_RequestChange C4_SettingsScreen_HandleKeyboardInput(void* screenData, SDL_Scancode scancode) {
    if (!screenData) {
        return C4_ScreenChangeRequest_None;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    return C4_ScreenChangeRequest_None;
}

C4_Screen_RequestChange C4_SettingsScreen_HandleMouseEvents(void* screenData, SDL_Event* event) {
    if (!screenData || !event) {
        return C4_ScreenChangeRequest_None;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    return C4_Button_HandleMouseEvents(screen->backButton, event, screen->renderer);
}