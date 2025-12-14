#pragma once
#include "Connect4/ui/rectangle_element.h"
#include "Connect4/ui/text_element.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/ui/screens/interface.h"
#include <stdbool.h>

typedef struct {
    SDL_Color background;
    SDL_Color text;
} C4_ButtonColorInfo;

typedef struct {
    C4_RectangleUIElement* background;
    C4_TextUIElement* text;
    C4_ButtonColorInfo defaultColors;
    C4_ButtonColorInfo hoverColors;
    C4_ButtonColorInfo clickColors;
    bool isHovered;
    bool isPressed;
    C4_ScreenChangeRequest onClick;
} C4_Button;

C4_Button* C4_Button_Create(
    SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, const SDL_FRect background, const C4_ButtonColorInfo defaultColors,
    const C4_ButtonColorInfo hoverColors, const C4_ButtonColorInfo clickColors, C4_ScreenChangeRequest onClick
);
void C4_Button_Destroy(C4_Button* button);
void C4_Button_Draw(C4_Button* button, SDL_Renderer* renderer);
C4_ScreenChangeRequest C4_Button_HandleMouseEvents(C4_Button* button, SDL_Event* event, SDL_Renderer* renderer);
void C4_Button_CenterTextInBackground(C4_Button* button, C4_Axis axis);
void C4_Button_CenterInWindow(C4_Button* button, C4_Axis axis);