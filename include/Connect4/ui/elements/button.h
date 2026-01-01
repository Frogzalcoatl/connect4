#pragma once
#include "Connect4/ui/elements/rectangle.h"
#include "Connect4/ui/elements/text.h"
#include "Connect4/ui/elements/borders.h"
#include "Connect4/ui/elements/symbol.h"
#include "Connect4/assets/fonts.h"
#include "Connect4/ui/themes.h"
#include <stdbool.h>

typedef void (*C4_UI_Callback)(void* context);

typedef struct {
    C4_UI_Rectangle background;
    C4_UI_Text text;
    C4_UI_Borders borders;
    C4_UI_Symbol symbol;
    C4_UI_Button_ColorInfo defaultColors;
    C4_UI_Button_ColorInfo hoverColors;
    C4_UI_Button_ColorInfo pressedColors;
    C4_UI_Button_ColorInfo inactiveColors;
    bool isHovered;
    bool isPressed;
    bool isActive;
    bool resetHoverOnClick;
    C4_UI_Callback OnClick;
    void* OnClickContext;
} C4_UI_Button;

bool C4_UI_Button_InitProperties(
    C4_UI_Button* button, SDL_Renderer* renderer, const char* str, const SDL_FRect destination, C4_UI_SymbolType symbol,
    float symbolWidth, float symbolHeight, int symbolRotationDegrees, const C4_UI_Theme* theme, C4_UI_Callback OnClick, void* OnClickContext
);
C4_UI_Button* C4_UI_Button_Create(
    SDL_Renderer* renderer, const char* str, const SDL_FRect destination, C4_UI_SymbolType symbol,
    float symbolWidth, float symbolHeight, int symbolRotationDegrees, const C4_UI_Theme* theme, C4_UI_Callback OnClick, void* OnClickContext
);
void C4_UI_Button_FreeResources(C4_UI_Button* button);
void C4_UI_Button_Destroy(C4_UI_Button* button);
void C4_UI_Button_Draw(C4_UI_Button* button, SDL_Renderer* renderer);
void C4_UI_Button_HandleMouseEvents(C4_UI_Button* button, SDL_Event* event);
void C4_UI_Button_CenterElementsInBackground(C4_UI_Button* button, C4_Axis axis);
void C4_UI_Button_CenterInWindow(C4_UI_Button* button, C4_Axis axis);
void C4_UI_Button_TransformResize(C4_UI_Button* button, float x, float y, float w, float h);
void C4_UI_Button_UpdateStr(C4_UI_Button* button, const char* str, SDL_Renderer* renderer);
void C4_UI_Button_Reset(void* button);