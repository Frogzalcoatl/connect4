#include "Connect4/ui/screens/menu.h"
#include "Connect4/assets/fonts.h"
#include <stdlib.h>

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
    screen->title = C4_UI_Text_Create(screen->renderer, "Connect4", C4_FontType_Bold, 100.f, 0.f, 0.f, 0);
    if (!screen->title) {
        SDL_Log("Unable to create title text element");
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    C4_UI_Text_CenterInWindow(screen->title, C4_Axis_X);
    screen->playButton = C4_UI_Button_Create(
        screen->renderer, "Play", C4_FontType_Bold, 32.f,
        (SDL_FRect){0.f, 380.f, 400.f, 100.f }
    );
    if (!screen->playButton) {
        SDL_Log("Unable to create play button");
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    C4_UI_Button_CenterInWindow(screen->playButton, C4_Axis_X);
    screen->settingsButton = C4_UI_Button_Create(
        screen->renderer, "Settings", C4_FontType_Bold, 32.f,
        (SDL_FRect){0.f, 0.f, 400.f, 100.f }
    );
    if (!screen->settingsButton) {
        SDL_Log("Unable to create settings button");
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    C4_UI_Button_CenterInWindow(screen->settingsButton, C4_Axis_XY);
    screen->quitButton = C4_UI_Button_Create(
        screen->renderer, "Quit", C4_FontType_Bold, 32.f,
        (SDL_FRect){0.f, 600.f, 400.f, 100.f }
    );
    if (!screen->quitButton) {
        SDL_Log("Unable to create quit button");
        C4_Screen_Menu_Destroy(screen);
        return NULL;
    }
    C4_UI_Button_CenterInWindow(screen->quitButton, C4_Axis_X);
    return screen;
}

void C4_Screen_Menu_Destroy(void* screenData) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    C4_UI_Button_Destroy(screen->quitButton);
    C4_UI_Button_Destroy(screen->settingsButton);
    C4_UI_Button_Destroy(screen->playButton);
    C4_UI_Text_Destroy(screen->title);
    free(screen);
}

void C4_Screen_Menu_Draw(void* screenData) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    C4_UI_Button_Draw(screen->playButton, screen->renderer);
    C4_UI_Button_Draw(screen->settingsButton, screen->renderer);
    C4_UI_Button_Draw(screen->quitButton, screen->renderer);
    C4_UI_Text_Draw(screen->title, screen->renderer);
}

C4_Screen_RequestChange C4_Screen_Menu_HandleKeyboardInput(void* screenData, SDL_Scancode scancode) {
    if (!screenData) {
        SDL_Log("Menu screen is NULL");
        return C4_Screen_RequestChange_None;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    // Will probably create some kind of struct for a popup that appears when pressing esc to close the game
    return C4_Screen_RequestChange_None;
}

C4_Screen_RequestChange C4_Screen_Menu_HandleMouseEvents(void* screenData, SDL_Event* event) {
    // lmao ill improve this later just trying to make sure everything works. Will probably use an array of button pointers or smth and iterate through them
    if (!screenData || !event) {
        SDL_Log("Menu screen and/or event is NULL");
        return C4_Screen_RequestChange_None;
    }
    C4_Screen_Menu* screen = (C4_Screen_Menu*)screenData;
    if (C4_UI_Button_HandleMouseEvents(screen->playButton, event, screen->renderer)) {
        return C4_Screen_RequestChange_Game;
    }
    if (C4_UI_Button_HandleMouseEvents(screen->settingsButton, event, screen->renderer)) {
        return C4_Screen_RequestChange_Settings;
    }
    if (C4_UI_Button_HandleMouseEvents(screen->quitButton, event, screen->renderer)) {
        return C4_Screen_RequestChange_CloseWindow;
    }
    return C4_Screen_RequestChange_None;
}