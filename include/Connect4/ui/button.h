#pragma once
#include "Connect4/ui/rectangle_element.h"
#include "Connect4/ui/text_element.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/ui/screens/interface.h"
#include <stdbool.h>

typedef struct {
    SDL_Color background;
    SDL_Color text;
} C4_UI_ButtonColorInfo;

typedef struct {
    C4_UI_Rectangle* background;
    C4_UI_Text* text;
    C4_UI_ButtonColorInfo defaultColors;
    C4_UI_ButtonColorInfo hoverColors;
    C4_UI_ButtonColorInfo clickColors;
    bool isHovered;
    bool isPressed;
    C4_Screen_RequestChange onClick;
} C4_UI_Button;

C4_UI_Button* C4_Button_Create(
    SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, const SDL_FRect background, const C4_UI_ButtonColorInfo defaultColors,
    const C4_UI_ButtonColorInfo hoverColors, const C4_UI_ButtonColorInfo clickColors, C4_Screen_RequestChange onClick
);
void C4_Button_Destroy(C4_UI_Button* button);
void C4_Button_Draw(C4_UI_Button* button, SDL_Renderer* renderer);
C4_Screen_RequestChange C4_Button_HandleMouseEvents(C4_UI_Button* button, SDL_Event* event, SDL_Renderer* renderer);
void C4_Button_CenterTextInBackground(C4_UI_Button* button, C4_Axis axis);
void C4_Button_CenterInWindow(C4_UI_Button* button, C4_Axis axis);