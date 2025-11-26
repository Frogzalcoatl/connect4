#include "Connect4/ui/screens/menu.h"
#include "Connect4/ui/fontManager.h"
#include <stdlib.h>

C4_MenuScreen* C4_MenuScreen_Create(SDL_Renderer* renderer) {
    if (!renderer) {
        return NULL;
    }
    C4_MenuScreen* screen = calloc(1, sizeof(C4_MenuScreen));
    if (!screen) {
        return NULL;
    }
    screen->renderer = renderer;
    screen->title = C4_TextUIElement_Create(screen->renderer, "Connect4", C4_FontType_Bold, 100.f, (SDL_Color){255, 255, 255, 255}, 0.f, 0.f);
    if (!screen->title) {
        C4_MenuScreen_Destroy(screen);
        return NULL;
    }
    C4_TextUIElement_CenterInWindow(screen->title, C4_Axis_X);
    screen->playButton = C4_Button_Create(
        screen->renderer, "Play", C4_FontType_Bold, 32.f,
        (SDL_FRect){0.f, 380.f, 400.f, 100.f },
        (C4_ButtonColorInfo){(SDL_Color){150, 150, 150, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_ButtonColorInfo){(SDL_Color){200, 200, 200, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_ButtonColorInfo){(SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255}},
        C4_ScreenChangeRequest_Game
    );
    if (!screen->playButton) {
        C4_MenuScreen_Destroy(screen);
        return NULL;
    }
    C4_Button_CenterInWindow(screen->playButton, C4_Axis_X);
    screen->settingsButton = C4_Button_Create(
        screen->renderer, "Settings", C4_FontType_Bold, 32.f,
        (SDL_FRect){0.f, 0.f, 400.f, 100.f },
        (C4_ButtonColorInfo){(SDL_Color){150, 150, 150, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_ButtonColorInfo){(SDL_Color){200, 200, 200, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_ButtonColorInfo){(SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255}},
        C4_ScreenChangeRequest_Settings
    );
    if (!screen->settingsButton) {
        C4_MenuScreen_Destroy(screen);
        return NULL;
    }
    C4_Button_CenterInWindow(screen->settingsButton, C4_Axis_XY);
    screen->quitButton = C4_Button_Create(
        screen->renderer, "Quit", C4_FontType_Bold, 32.f,
        (SDL_FRect){0.f, 600.f, 400.f, 100.f },
        (C4_ButtonColorInfo){(SDL_Color){150, 150, 150, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_ButtonColorInfo){(SDL_Color){200, 200, 200, 255}, (SDL_Color){255, 255, 255, 255}},
        (C4_ButtonColorInfo){(SDL_Color){255, 255, 255, 255}, (SDL_Color){0, 0, 0, 255}},
        C4_ScreenChangeRequest_CloseWindow
    );
    if (!screen->quitButton) {
        C4_MenuScreen_Destroy(screen);
        return NULL;
    }
    C4_Button_CenterInWindow(screen->quitButton, C4_Axis_X);
    return screen;
}

void C4_MenuScreen_Destroy(C4_MenuScreen* screen) {
    if (!screen) {
        return;
    }
    C4_Button_Destroy(screen->quitButton);
    C4_Button_Destroy(screen->settingsButton);
    C4_Button_Destroy(screen->playButton);
    C4_TextUIElement_Destroy(screen->title);
    free(screen);
}

void C4_MenuScreen_Draw(C4_MenuScreen* screen) {
    if (!screen) {
        return;
    }
    C4_Button_Draw(screen->playButton, screen->renderer);
    C4_Button_Draw(screen->settingsButton, screen->renderer);
    C4_Button_Draw(screen->quitButton, screen->renderer);
    C4_TextUIElement_Draw(screen->title, screen->renderer);
}

C4_ScreenChangeRequest C4_MenuScreen_HandleKeyboardInput(C4_MenuScreen* screen, SDL_Scancode scancode) {
    if (!screen) {
        return C4_ScreenChangeRequest_None;
    }
    // Will probably create some kind of struct for a popup that appears when pressing esc to close the game
    return C4_ScreenChangeRequest_None;
}

C4_ScreenChangeRequest C4_MenuScreen_HandleMouseEvents(C4_MenuScreen* screen, SDL_Event* event) {
    // lmao ill improve this later just trying to make sure everything works. Will probably use an array of button pointers or smth and iterate through them
    if (!screen || !event) {
        return C4_ScreenChangeRequest_None;
    }
    C4_ScreenChangeRequest request = C4_ScreenChangeRequest_None;
    C4_ScreenChangeRequest newRequest = C4_Button_HandleMouseEvents(screen->playButton, event, screen->renderer);
    if (newRequest != C4_ScreenChangeRequest_None) {
        request = newRequest;
    }
    newRequest = C4_Button_HandleMouseEvents(screen->settingsButton, event, screen->renderer);
    if (newRequest != C4_ScreenChangeRequest_None) {
        request = newRequest;
    }
    newRequest = C4_Button_HandleMouseEvents(screen->quitButton, event, screen->renderer);
    if (newRequest != C4_ScreenChangeRequest_None) {
        request = newRequest;
    }
    return request;
}