#pragma once
#include "Connect4/ui/elements/rectangle.h"
#include "Connect4/ui/elements/text.h"
#include "Connect4/ui/elements/borders.h"
#include "Connect4/ui/elements/symbol.h"
#include "Connect4/ui/themes.h"
#include <stdbool.h>

typedef void (*C4_UI_Callback)(void* context);

typedef struct {
    const char* text;
    SDL_FRect destination;
    C4_UI_SymbolType symbol;
    TTF_Font* font;
    const C4_UI_Theme* theme;
} C4_UI_Button_Config;

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
    C4_UI_Callback OnReleaseCallback;
    void* OnClickContext;
    C4_UI_Callback WhilePressedCallback;
    void* WhilePressedContext;
    C4_UI_Callback OnPressCallback;
    void* OnPressContect;
    C4_UI_Callback OnHoverCallback;
    void* OnHoverContext;
    float delay;
    float interval;
    float pressTimer;
    bool isRepeating;
} C4_UI_Button;

bool C4_UI_Button_InitProperties(C4_UI_Button* button, SDL_Renderer* renderer, const C4_UI_Button_Config* config);
C4_UI_Button* C4_UI_Button_Create(SDL_Renderer* renderer, const C4_UI_Button_Config* config);
void C4_UI_Button_FreeResources(C4_UI_Button* button);
void C4_UI_Button_Destroy(void* data);
void C4_UI_Button_Draw(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY);
void C4_UI_Button_Update(void* data, float deltaTime);
void C4_UI_Button_HandleMouseEvents(void* data, SDL_Event* event, float scale, float parentX, float parentY);
void C4_UI_Button_CenterElementsInBackground(C4_UI_Button* button, C4_Axis axis);
void C4_UI_Button_CenterInWindow(C4_UI_Button* button, C4_Axis axis, unsigned int windowWidth, unsigned int windowHeight);
void C4_UI_Button_TransformResize(C4_UI_Button* button, float x, float y, float w, float h);
void C4_UI_Button_UpdateStr(C4_UI_Button* button, const char* str, SDL_Renderer* renderer);
void C4_UI_Button_Reset(void* data);
void C4_UI_Button_SetTheme(C4_UI_Button* button, const C4_UI_Theme* theme);

typedef enum {
    C4_UI_Button_CallbackType_OnRelease = 1,
    C4_UI_Button_CallbackType_WhilePressed,
    C4_UI_Button_CallbackType_OnPress,
    C4_UI_Button_CallbackType_OnHover,
    C4_UI_Button_CallbackType_Length
} C4_UI_Button_CallbackType;

void C4_UI_Button_SetPostCallback(C4_UI_Button_CallbackType type, C4_UI_Callback callback, void* context);