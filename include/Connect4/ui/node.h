#pragma once
#include "Connect4/ui/interaction.h"
#include "Connect4/ui/themes.h"
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

typedef enum {
    C4_UI_Type_Container,
    C4_UI_Type_Text,
    C4_UI_Type_Shape
} C4_UI_Type;

typedef enum {
    C4_UI_Shape_Rectangle = 1,
    C4_UI_Shape_Triangle,
    C4_UI_Shape_Circle
} C4_UI_ShapeType;

typedef struct {
    TTF_Font* font;
    TTF_Text* textObject;
} C4_UI_Data_Text;

typedef struct {
    C4_UI_ShapeType type;
    float rotationDegrees;
    unsigned int borderWidth;
} C4_UI_Data_Shape;

typedef enum {
    C4_UI_Direction_Horizontal,
    C4_UI_Direction_Vertical
} C4_UI_Direction;

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
    bool fitContent; // If true, rect expands to fit children

    struct C4_UI_Node* navUp;
    struct C4_UI_Node* navDown;
    struct C4_UI_Node* navLeft;
    struct C4_UI_Node* navRight;
} C4_UI_Node;

typedef struct {
    C4_UI_ShapeType type;
    unsigned int borderWidth;
} C4_UI_Data_Shape_Config;

typedef struct {
    char* text;
    TTF_Font* font;
    TTF_TextEngine* textEngine;
} C4_UI_Data_Text_Config;

typedef struct C4_UI_Node_Config {
    C4_UI_Type type;
    C4_UI_Style* style;
    SDL_FRect rect;
    union {
        C4_UI_Data_Shape_Config shape;
        C4_UI_Data_Text_Config text;
    };
} C4_UI_Node_Config;

void C4_UI_Node_Destroy(C4_UI_Node* node);
void C4_UI_Node_Draw(C4_UI_Node* node, SDL_Renderer* renderer, float scale, float parentX, float parentY);
C4_UI_Node* C4_UI_Node_FindFocusable(C4_UI_Node* node);
bool C4_UI_Node_HandleAction(C4_UI_Node* node, C4_InputEvent event);
bool C4_UI_Node_HandleMouseEvents(C4_UI_Node* node, SDL_Event* event);
void C4_UI_Node_Update(C4_UI_Node* node, float deltaTime);
void C4_UI_Node_AttachChild(C4_UI_Node* parent, C4_UI_Node* child);
void C4_UI_Node_PushNode(C4_UI_Node* head, C4_UI_Node* newNode);
C4_UI_Node* C4_UI_Node_Create(C4_UI_Node_Config* config, float UIScale);
void C4_UI_Node_SetTextString(C4_UI_Node* node, const char* newString);
void C4_UI_Node_ChangeFont(C4_UI_Node* node, TTF_Font* newFont);
void C4_UI_Node_SetTextWrap(C4_UI_Node* node, int widthInPixels);

typedef enum {
    C4_UI_Align_TopLeft,
    C4_UI_Align_Top,
    C4_UI_Align_TopRight,
    C4_UI_Align_CenterLeft,
    C4_UI_Align_Center,
    C4_UI_Align_CenterRight,
    C4_UI_Align_BottomLeft,
    C4_UI_Align_Bottom,
    C4_UI_Align_BottomRight
} C4_UI_Align;
void C4_UI_Node_AlignChildren(C4_UI_Node* node, C4_UI_Align align);