#pragma once
#include "SDL3/SDL.h"
#include "Connect4/ui/canvas.h"
#include <stdlib.h>

typedef enum C4_ScreenType {
    C4_ScreenType_Menu,
    C4_ScreenType_Game,
    C4_ScreenType_Settings,
    C4_ScreenType_ScreenCount
} C4_ScreenType;

typedef enum C4_UI_LayoutType {
    C4_UI_LayoutType_None,
    C4_UI_LayoutType_Wide,
    C4_UI_LayoutType_Tall,
    C4_UI_LayoutType_Count
} C4_UI_LayoutType;

C4_UI_LayoutType C4_UI_GetCurrentLayout(unsigned int windowWidth, unsigned int windowHeight);

// Forward declaration so it can be inside the struct
typedef struct C4_UI_Screen C4_UI_Screen;

struct C4_UI_Screen {
    void* data;
    void (*Update)(C4_UI_Screen* screen, float deltaTime, float UIScale);
    void (*Draw)(C4_UI_Screen* screen, float UIScale);
    void (*OnEnter)(C4_UI_Screen* screen);
    void (*HandleWindowResize)(C4_UI_Screen* screen, C4_UI_LayoutType layout);
    void (*HandleEvent)(C4_UI_Screen* screen, SDL_Window* window, SDL_Event* event);
    void (*Destroy)(C4_UI_Screen* screen);
    C4_UI_Canvas canvas;
};

C4_UI_Screen* C4_Screen_Create(SDL_Renderer* renderer, TTF_TextEngine* textEngine);

void C4_Screen_Update_Default(C4_UI_Screen* screen, float deltaTime, float UIScale);
void C4_Screen_Draw_Default(C4_UI_Screen* screen, float UIScale);
void C4_UI_Screen_HandleEvent_Default(C4_UI_Screen* screen, SDL_Window* window, SDL_Event* event);
void C4_UI_Screen_Destroy_Default(C4_UI_Screen* screen);
