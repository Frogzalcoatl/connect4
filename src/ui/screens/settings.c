#include "Connect4/ui/screens/settings.h"
#include "Connect4/ui/fontManager.h"
#include <stdlib.h>

C4_SettingsScreen* C4_SettingsScreen_Create(SDL_Window* window, SDL_Renderer* renderer) {
    if (!window || !renderer) {
        return NULL;
    }
    C4_SettingsScreen* screen = calloc(1, sizeof(C4_SettingsScreen));
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
        (C4_ButtonColorInfo){(SDL_Color){150, 150, 150, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_ButtonColorInfo){(SDL_Color){200, 200, 200, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_ButtonColorInfo){(SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255}},
        C4_ScreenChangeRequest_Menu
    );
    C4_Button_CenterInWindow(screen->backButton, C4_Axis_X);
    return screen;
}

void C4_SettingsScreen_Destroy(C4_SettingsScreen* screen) {
    if (!screen) {
        return;
    }
    C4_TextUIElement_Destroy(screen->title);
    free(screen);
}

void C4_SettingsScreen_Draw(C4_SettingsScreen* screen) {
    if (!screen) {
        return;
    }
    C4_Button_Draw(screen->backButton, screen->renderer);
    C4_TextUIElement_Draw(screen->title, screen->renderer);
}

C4_ScreenChangeRequest C4_SettingsScreen_HandleKeyboardInput(C4_SettingsScreen* screen, SDL_Scancode scancode) {
    if (!screen) {
        return C4_ScreenChangeRequest_None;
    }
    return C4_ScreenChangeRequest_None;
}

C4_ScreenChangeRequest C4_SettingsScreen_HandleMouseEvents(C4_SettingsScreen* screen, SDL_Event* event) {
    if (!screen || !event) {
        return C4_ScreenChangeRequest_None;
    }
    return C4_Button_HandleMouseEvents(screen->backButton, event, screen->renderer);
}