#pragma once
#include "Connect4/ui/node.h"

typedef struct {
    C4_UI_Style* style;
    SDL_FRect rect;
    C4_UI_ShapeType shapeType;
    unsigned int borderWidth;
    char* text;
    TTF_Font* font;
    TTF_TextEngine* textEngine;
} C4_UI_Button_Config;

C4_UI_Node* C4_UI_Button_Create(C4_MemoryArena* arena, C4_UI_Button_Config* config);

typedef struct {
    float posX;
    float posY;
    C4_UI_Direction direction;
    float spacing;
    float padding;
    C4_UI_Button_Config* buttonsArr;
    size_t buttonsArrSize;
} C4_UI_Buttons_Config;
C4_UI_Node* C4_UI_Buttons_Create(C4_MemoryArena* arena, C4_UI_Buttons_Config* config);
void C4_UI_Buttons_CreateConfigArr(C4_UI_Button_Config* defaultConfig, char* buttonStrings[], size_t buttonCount, C4_UI_Button_Config returnValue[]);