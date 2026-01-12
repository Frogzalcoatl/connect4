#pragma once
#include "Connect4/ui/memoryArena.h"
#include "Connect4/ui/node.h"
#include "SDL3/SDL.h"

typedef struct {
    SDL_Renderer* renderer;
    TTF_TextEngine* textEngine;
    C4_UI_Node* root;
    C4_UI_Node* focusedNode;

    C4_MemoryArena arena;

    float offsetX;
    float offsetY;
} C4_UI_Canvas;

void C4_UI_Canvas_Init(C4_UI_Canvas* canvas, SDL_Renderer* renderer, TTF_TextEngine* textEngine, float offsetX, float offsetY);
void C4_UI_Canvas_Destroy(C4_UI_Canvas* canvas);
void C4_UI_Canvas_Draw(C4_UI_Canvas* canvas, float scale);
void C4_UI_Canvas_HandleEvent(C4_UI_Canvas* canvas, SDL_Event* event, float scale);
void C4_UI_Canvas_Update(C4_UI_Canvas* canvas, float deltaTime);
void C4_UI_Canvas_ResetInteractions(C4_UI_Canvas* canvas);
void C4_UI_Canvas_AddNode(C4_UI_Canvas* canvas, C4_UI_Node* node);