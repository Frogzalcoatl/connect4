#pragma once
#include <SDL3/SDL.h>
#include "Connect4/ui/canvas.h"
#include <stdlib.h>

// Forward declaration so it can be inside the struct
typedef struct C4_UI_Screen C4_UI_Screen;

struct C4_UI_Screen {
    void* data;
    void (*Update)(C4_UI_Screen* screen, float deltaTime);
    void (*Draw)(C4_UI_Screen* screen, float UIScale);
    void (*OnEnter)(C4_UI_Screen* screen);
    void (*HandleWindowResize)(C4_UI_Screen* screen, C4_UI_LayoutType layout);
    void (*HandleEvent)(C4_UI_Screen* screen, SDL_Event* event, float UIScale);
    void (*Destroy)(C4_UI_Screen* screen);
    C4_UI_Canvas canvas;
};

C4_UI_Screen* C4_Screen_Create(SDL_Renderer* renderer);