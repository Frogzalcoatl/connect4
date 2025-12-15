#include "Connect4/ui/screens/menu.h"
#include "Connect4/assets/fonts.h"
#include <stdlib.h>

#define C4_SCREEN_MENU_BUTTONCOUNT 4

static const char buttonText[C4_SCREEN_MENU_BUTTONCOUNT][16] = {
    "1 Player",
    "2 Players",
    "Settings",
    "Quit"
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
    if (!C4_UI_Text_InitProperties(&screen->title, screen->renderer, "Connect4", C4_FontType_Bold, 100.f, 0.f, 0.f, 0)) {
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    C4_UI_CenterInWindow(&screen->title.destination, C4_Axis_X);
    if (!C4_UI_ButtonStack_InitProperties(&screen->buttonStack, screen->renderer, (SDL_FRect){0.f, 0.f, 700.f, 500.f}, C4_SCREEN_MENU_BUTTONCOUNT, C4_UI_ButtonStack_Direction_Vertical, 15, 32.f)) {
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    for (size_t i = 0; i < C4_SCREEN_MENU_BUTTONCOUNT; i++) {
        C4_UI_ButtonStack_SetButtonIndex(&screen->buttonStack, i, screen->renderer, buttonText[i], C4_FontType_Bold, 32.f);
    }
    C4_UI_ButtonStack_CenterInWindow(&screen->buttonStack, C4_Axis_XY);
    return screen;
}

void C4_Screen_Menu_Destroy(void* screenData) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    C4_UI_ButtonStack_FreeResources(&screen->buttonStack);
    C4_UI_Text_FreeResources(&screen->title);
    free(screen);
}

void C4_Screen_Menu_Draw(void* screenData) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    C4_UI_ButtonStack_Draw(&screen->buttonStack, screen->renderer);
    C4_UI_Text_Draw(&screen->title, screen->renderer);
}

C4_Screen_RequestChange C4_Screen_Menu_HandleKeyboardInput(void* screenData, SDL_Scancode scancode) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return C4_Screen_RequestChange_None;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    // popup for esc to exit the game
    return C4_Screen_RequestChange_None;
}

C4_Screen_RequestChange C4_Screen_Menu_HandleMouseEvents(void* screenData, SDL_Event* event) {
    if (!screenData || !event) {
        SDL_Log("Menu screen and/or event is NULL");
        return C4_Screen_RequestChange_None;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    switch (C4_UI_ButtonStack_HandleMouseEvents(&screen->buttonStack, event, screen->renderer)) {
        // 1 Player Button
        case 0: return C4_Screen_RequestChange_Game;
        //2 Players button
        case 1: return C4_Screen_RequestChange_Game;
        // Settings button
        case 2: return C4_Screen_RequestChange_Settings;
        // Quit button
        case 3: return C4_Screen_RequestChange_CloseWindow;
        default: return C4_Screen_RequestChange_None;
    }
}