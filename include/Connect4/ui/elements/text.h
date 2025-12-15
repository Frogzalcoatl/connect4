#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Connect4/ui/elements/index.h"
#include "Connect4/assets/fonts.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    TTF_Font* font;

    // [Owns] properties specific to this UIElement
    SDL_Color color;
    float ptSize;
    SDL_Texture* texture;
    SDL_FRect destination;
    int wrapWidth;
    char str[512];
} C4_UI_Text;

C4_UI_Text* C4_UI_Text_Create(SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, float destinationX, float destinationY, int wrapWidth);
void C4_UI_Text_Destroy(C4_UI_Text* element);
void C4_UI_Text_ChangeStr(C4_UI_Text* element, const char* newStr);
void C4_UI_Text_Draw(C4_UI_Text* element, SDL_Renderer* renderer);
void C4_UI_Text_Refresh(C4_UI_Text* element, SDL_Renderer* renderer);
void C4_UI_Text_CenterInWindow(C4_UI_Text* element, C4_Axis axis);