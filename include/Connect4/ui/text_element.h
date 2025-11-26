#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Connect4/ui/rectangle_element.h"
#include "Connect4/ui/fontManager.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    TTF_Font* font;

    // [Owns] properties specific to this UIElement
    SDL_Color color;
    float ptSize;
    SDL_Texture* texture;
    SDL_FRect destination;
    char str[512];
} C4_TextUIElement;

C4_TextUIElement* C4_TextUIElement_Create(SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, SDL_Color color, float destinationX, float destinationY);
void C4_TextUIElement_Destroy(C4_TextUIElement* element);
void C4_TextUIElement_ChangeStr(C4_TextUIElement* element, const char* newStr);
void C4_TextUIElement_Draw(C4_TextUIElement* element, SDL_Renderer* renderer);
void C4_TextUIElement_Refresh(C4_TextUIElement* element, SDL_Renderer* renderer);
void C4_TextUIElement_CenterInWindow(C4_TextUIElement* element, C4_Axis axis);