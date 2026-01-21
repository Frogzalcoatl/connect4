#include "Connect4/ui/screens/screen.h"
#include "Connect4/game/consoleOutput.h"
#include <assert.h>

void C4_Screen_Update_Default(C4_UI_Screen* screen, float deltaTime, float UIScale) {
    assert(screen);

    C4_UI_Canvas_Update(&screen->canvas, deltaTime, UIScale);
}

void C4_Screen_Draw_Default(C4_UI_Screen* screen, float UIScale) {
    assert(screen);

    C4_UI_Canvas_Draw(&screen->canvas, UIScale);
}

void C4_UI_Screen_HandleEvent_Default(C4_UI_Screen* screen, SDL_Window* window, SDL_Event* event) {
    assert(screen && window && event);

    C4_UI_Canvas_HandleEvent(&screen->canvas, window, event);
}

void C4_UI_Screen_Destroy_Default(C4_UI_Screen* screen) {
    if (!screen) {
        return;
    }
    C4_UI_Canvas_Destroy(&screen->canvas);
    if (screen->data) {
        SDL_free(screen->data);
    }
    SDL_free(screen);
}

C4_UI_Screen* C4_Screen_Create(SDL_Renderer* renderer, TTF_TextEngine* textEngine) {
    assert(renderer && textEngine);

    C4_UI_Screen* screen = SDL_calloc(1, sizeof(C4_UI_Screen));
    if (!screen) {
        C4_FatalError(C4_ErrorCode_OutOfMemory, "Unable to allocate memory for screen");
    }

    C4_UI_Canvas_Init(&screen->canvas, renderer, textEngine, 0.f, 0.f);

    screen->Update = C4_Screen_Update_Default;
    screen->Draw = C4_Screen_Draw_Default;
    screen->HandleEvent = C4_UI_Screen_HandleEvent_Default;
    screen->Destroy = C4_UI_Screen_Destroy_Default;
    
    // These are specific to each screen
    screen->OnEnter = NULL;
    screen->HandleWindowResize = NULL;
    screen->data = NULL;

    return screen;
}

C4_UI_LayoutType C4_UI_GetCurrentLayout(unsigned int windowWidth, unsigned int windowHeight) {
    if (windowWidth >= windowHeight) {
        return C4_UI_LayoutType_Wide;
    } else {
        return C4_UI_LayoutType_Tall;
    }
}