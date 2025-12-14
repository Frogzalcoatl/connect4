#pragma once
#include <SDL3/SDL.h>

typedef enum {
    C4_ScreenChangeRequest_None,
    C4_ScreenChangeRequest_Menu,
    C4_ScreenChangeRequest_Settings,
    C4_ScreenChangeRequest_Game,
    C4_ScreenChangeRequest_CloseWindow
} C4_Screen_RequestChange;

typedef struct {
    // A generic pointer that can hold a C4_Screen_Menu, C4_Gamescreen, or anything else
    void* data;
    void (*Draw)(void* screenData);
    C4_Screen_RequestChange (*HandleMouseEvents)(void* screenData, SDL_Event* event);
    C4_Screen_RequestChange (*HandleKeyboardInput)(void* screenData, SDL_Scancode scancode);
    void (*Destroy)(void* screenData);
} C4_Screen_Interface;

// Its easy to be tempted to use function pointers for every struct
// But the memory required to store all those pointers can build up over time
// Only use them in cases like these where you know you will have very few instances of the struct (In this project 1 instance)