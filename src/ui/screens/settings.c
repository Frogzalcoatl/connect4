#include "Connect4/ui/screens/settings.h"
#include "Connect4/assets/fonts.h"
#include <stdlib.h>

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
    screen->title = C4_UI_Text_Create(screen->renderer, "Settings", C4_FontType_Bold, 100.f, (SDL_Color){255, 255, 255, 255}, 0.f, 0.f);
    if (!screen->title) {
        SDL_Log("Unable to create title text element");
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    C4_UI_Text_CenterInWindow(screen->title, C4_Axis_X);
    screen->backButton = C4_UI_Button_Create(
        screen->renderer, "Back", C4_FontType_Bold, 32.f,
        (SDL_FRect){0.f, 800.f, 400.f, 100.f },
        (C4_UI_ButtonColorInfo){(SDL_Color){150, 150, 150, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_UI_ButtonColorInfo){(SDL_Color){200, 200, 200, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_UI_ButtonColorInfo){(SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255}},
        C4_ScreenChangeRequest_Menu
    );
    if (!screen->backButton) {
        SDL_Log("Unable to create back button");
        C4_Screen_Settings_Destroy(screen);
        return NULL;
    }
    C4_UI_Button_CenterInWindow(screen->backButton, C4_Axis_X);
    return screen;
}

void C4_Screen_Settings_Destroy(void* screenData) {
    if (!screenData) {
        SDL_Log("Settings screen is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    C4_UI_Text_Destroy(screen->title);
    free(screen);
}

void C4_Screen_Settings_Draw(void* screenData) {
    if (!screenData) {
        SDL_Log("Settings screen is NULL");
        return;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    C4_UI_Button_Draw(screen->backButton, screen->renderer);
    C4_UI_Text_Draw(screen->title, screen->renderer);
}

C4_Screen_RequestChange C4_Screen_Settings_HandleKeyboardInput(void* screenData, SDL_Scancode scancode) {
    if (!screenData) {
        SDL_Log("Settings screen is NULL");
        return C4_ScreenChangeRequest_None;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    return C4_ScreenChangeRequest_None;
}

C4_Screen_RequestChange C4_Screen_Settings_HandleMouseEvents(void* screenData, SDL_Event* event) {
    if (!screenData || !event) {
        SDL_Log("Settings screen and/or event is NULL");
        return C4_ScreenChangeRequest_None;
    }
    C4_Screen_Settings* screen = (C4_Screen_Settings*)screenData;
    return C4_UI_Button_HandleMouseEvents(screen->backButton, event, screen->renderer);
}