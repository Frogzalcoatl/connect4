#include "Connect4/ui/screen.h"

static void C4_Screen_Update_Default(C4_UI_Screen* screen, float deltaTime) {
    if (!screen) {
        return;
    }
    C4_UI_Canvas_Update(&screen->canvas, deltaTime);
}

static void C4_Screen_Draw_Default(C4_UI_Screen* screen, float UIScale) {
    if (!screen) {
        return;
    }
    C4_UI_Canvas_Draw(&screen->canvas, UIScale);
}

static void C4_UI_Screen_HandleEvent_Default(C4_UI_Screen* screen, SDL_Event* event, float UIScale) {
    if (!screen || !event) {
        return;
    }
    C4_UI_Canvas_HandleEvent(&screen->canvas, event, UIScale);
}

static void C4_UI_Screen_Destroy_Default(C4_UI_Screen* screen) {
    if (!screen) {
        return;
    }
    C4_UI_Canvas_Clear(&screen->canvas);
    if (screen->data) {
        free(screen->data);
    }
    free(screen);
}

C4_UI_Screen* C4_Screen_Create(SDL_Renderer* renderer) {
    if (!renderer) {
        return NULL;
    }
    C4_UI_Screen* screen = calloc(1, sizeof(C4_UI_Screen));
    if (!screen) {
        SDL_Log("Unable to allocate memory for menu screen.");
        return NULL;
    }

    C4_UI_Canvas_Init(&screen->canvas, renderer, 0.f, 0.f);

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