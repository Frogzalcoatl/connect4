#pragma once
#include "Connect4/ui/elements/rectangle.h"
#include "Connect4/ui/elements/text.h"
#include "Connect4/ui/elements/borders.h"
#include "Connect4/ui/elements/symbol.h"
#include "Connect4/assets/fonts.h"
#include <stdbool.h>

typedef struct {
    SDL_Color background;
    SDL_Color text;
    SDL_Color borders;
    SDL_Color symbol;
} C4_UI_ButtonColorInfo;

typedef struct {
    C4_UI_Rectangle background;
    C4_UI_Text text;
    C4_UI_Borders borders;
    C4_UI_Symbol symbol;
    C4_UI_ButtonColorInfo defaultColors;
    C4_UI_ButtonColorInfo hoverColors;
    C4_UI_ButtonColorInfo clickColors;
    bool isHovered;
    bool isPressed;
} C4_UI_Button;

bool C4_UI_Button_InitProperties(
    C4_UI_Button* button, SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, const SDL_FRect background,
    unsigned int borderWidth, C4_UI_SymbolType symbol, float symbolWidth, float symbolHeight, int symbolRotationDegrees
);
C4_UI_Button* C4_UI_Button_Create(
    SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, const SDL_FRect background, unsigned int borderWidth,
    C4_UI_SymbolType symbol, float symbolWidth, float symbolHeight, int symbolRotationDegrees
);
void C4_UI_Button_FreeResources(C4_UI_Button* button);
void C4_UI_Button_Destroy(C4_UI_Button* button);
void C4_UI_Button_Draw(C4_UI_Button* button, SDL_Renderer* renderer);
bool C4_UI_Button_HandleMouseEvents(C4_UI_Button* button, SDL_Event* event);
void C4_UI_Button_CenterElementsInBackground(C4_UI_Button* button, C4_Axis axis);
void C4_UI_Button_CenterInWindow(C4_UI_Button* button, C4_Axis axis);
void C4_UI_Button_TransformResize(C4_UI_Button* button, float x, float y, float w, float h);
void C4_UI_Button_ChangeStr(C4_UI_Button* button, const char* str, SDL_Renderer* renderer);