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
} C4_UI_Symbol_Config;

typedef struct {
    C4_UI_SymbolType type;
    SDL_FRect destination;
    int rotationDegrees;
    SDL_Color color;
} C4_UI_Symbol;

bool C4_UI_Symbol_InitProperties(C4_UI_Symbol* symbol, const C4_UI_Symbol_Config* config);
C4_UI_Symbol* C4_UI_Symbol_Create(const C4_UI_Symbol_Config* config);
void C4_UI_Symbol_Destroy(void* data);
void C4_UI_Symbol_Draw(void* data, SDL_Renderer* renderer);