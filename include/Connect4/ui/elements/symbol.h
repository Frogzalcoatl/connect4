#pragma once
#include "SDL3/SDL.h"

typedef enum {
    C4_UI_SymbolType_None,
    C4_UI_SymbolType_Triangle,
    C4_UI_SymbolType_Star,
    C4_UI_SymbolType_Length
} C4_UI_SymbolType;

typedef struct {
    C4_UI_SymbolType type;
    SDL_FRect destination;
    int rotationDegrees;
    SDL_Color color;
} C4_UI_Symbol;

bool C4_UI_Symbol_InitProperties(C4_UI_Symbol* symbol, C4_UI_SymbolType type, const SDL_FRect destination, int rotationDegrees, const SDL_Color color);
C4_UI_Symbol* C4_UI_Symbol_Create(C4_UI_SymbolType type, const SDL_FRect destination, int rotationDegrees, const SDL_Color color);
void C4_UI_Symbol_Destroy(C4_UI_Symbol* symbol);
void C4_UI_Symbol_Draw(C4_UI_Symbol* symbol, SDL_Renderer* renderer);