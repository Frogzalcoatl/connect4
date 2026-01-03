#pragma once
#include "SDL3/SDL.h"
#include <SDL3_ttf/SDL_ttf.h>
#include "Connect4/ui/elements/index.h"

#define C4_UI_TEXT_STR_SIZE 1024

typedef struct {
    const char* str;
    TTF_Font* font;
    SDL_Color color;
    float ptSize;
    float destinationX;
    float destinationY;
    int wrapWidth;
} C4_UI_Text_Config;

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    TTF_Font* font;

    // [Owns] properties specific to this UIElement
    SDL_Color color;
    float ptSize;
    SDL_Texture* texture;
    SDL_FRect destination;
    int wrapWidth;
    char str[C4_UI_TEXT_STR_SIZE];
} C4_UI_Text;

bool C4_UI_Text_InitProperties(C4_UI_Text* element, SDL_Renderer* renderer, const C4_UI_Text_Config* config);
C4_UI_Text* C4_UI_Text_Create(SDL_Renderer* renderer, const C4_UI_Text_Config* config);
void C4_UI_Text_FreeResources(C4_UI_Text* element);
void C4_UI_Text_Destroy(void* data);
void C4_UI_Text_UpdateStr(C4_UI_Text* element, const char* newStr, SDL_Renderer* renderer);
void C4_UI_Text_Draw(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY);
void C4_UI_Text_ReloadTexture(C4_UI_Text* element, SDL_Renderer* renderer);