#pragma once
#include "Connect4/input/interaction.h"
#include "Connect4/ui/themes.h"
#include "Connect4/ui/memoryArena.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "Connect4/ui/types.h"

typedef struct C4_UI_Node {
    C4_UI_Type type;

    union {
        C4_UI_Data_Text text;
        C4_UI_Data_Shape shape;
    };

    struct C4_UI_Node* parent;
    struct C4_UI_Node* firstChild;
    struct C4_UI_Node* lastChild;
    struct C4_UI_Node* nextSibling;
    struct C4_UI_Node* prevSibling;

    C4_UI_Interaction input;
    C4_UI_Style style;

    bool inheritState;

    SDL_FRect rect;
    SDL_FRect absoluteRect;
    float padding;
    float spacing;
    C4_UI_Direction direction;
    C4_UI_Align childrenAlign;
    C4_UI_Align selfAlign;
    C4_UI_Mirror mirror;

    struct C4_UI_Node* navUp;
    struct C4_UI_Node* navDown;
    struct C4_UI_Node* navLeft;
    struct C4_UI_Node* navRight;
} C4_UI_Node;

typedef struct C4_UI_Data_Shape_Config {
    SDL_FRect rect;
    C4_UI_ShapeType type;
    unsigned int borderWidth;
} C4_UI_Data_Shape_Config;

typedef struct C4_UI_Data_Text_Config {
    float posX;
    float posY;
    float uiScale;
    char* text;
    TTF_Font* font;
    TTF_TextEngine* textEngine;
} C4_UI_Data_Text_Config;

typedef struct C4_UI_Node_Config {
    C4_UI_Type type;
    C4_UI_Style* style;
    union {
        C4_UI_Data_Shape_Config* shape;
        C4_UI_Data_Text_Config* text;
    };
} C4_UI_Node_Config;

void C4_UI_Node_CalculateLayout(C4_UI_Node* node, float uiScale, float parentX, float parentY);
void C4_UI_Node_Draw(C4_UI_Node* node, SDL_Renderer* renderer, float uiScale);
C4_UI_Node* C4_UI_Node_FindFocusable(C4_UI_Node* node);
bool C4_UI_Node_HandleAction(C4_UI_Node* node, C4_InputEvent event);
bool C4_UI_Node_HandleMouseEvents(C4_UI_Node* node, SDL_Event* event);
void C4_UI_Node_Update(C4_UI_Node* node, float deltaTime);
void C4_UI_Node_AttachChild(C4_UI_Node* parent, C4_UI_Node* child);
void C4_UI_Node_PushNode(C4_UI_Node* head, C4_UI_Node* newNode);
void C4_UI_Node_Reset(C4_UI_Node* node);
void C4_UI_Node_CleanupResources(C4_UI_Node* node);
C4_UI_Node* C4_UI_Node_Create(C4_MemoryArena* arena, C4_UI_Node_Config* config);
void C4_UI_Node_SetTextString(C4_UI_Node* node, const char* newString);
void C4_UI_Node_ChangeFont(C4_UI_Node* node, TTF_Font* newFont);
void C4_UI_Node_SetTextWrap(C4_UI_Node* node, int widthInPixels);

void C4_UI_Node_AlignChildren(C4_UI_Node* node, C4_UI_Axis axis);
void C4_UI_Node_ApplyChildSpacing(C4_UI_Node* parent);
void C4_UI_Node_ClampToWindow(C4_UI_Node* node, unsigned int windowWidth, unsigned int windowHeight);
void C4_UI_Node_AlignInParent(C4_UI_Node* node, SDL_FRect parentRect);
void C4_UI_Node_RefreshLayout(C4_UI_Node* node, SDL_FRect parentRect);