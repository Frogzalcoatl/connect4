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

C4_UI_Node* C4_UI_Button_Create(C4_UI_Button_Config* config, float UIScale);