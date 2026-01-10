#pragma once
#include "SDL3/SDL.h"
#include "Connect4/ui/elements/borders.h"
#include "Connect4/ui/elements/button.h"
#include "Connect4/ui/elements/buttonGroup.h"
#include "Connect4/ui/elements/numberInput.h"
#include "Connect4/ui/elements/popup.h"
#include "Connect4/ui/elements/rectangle.h"
#include "Connect4/ui/elements/symbol.h"
#include "Connect4/ui/elements/text.h"

typedef enum {
    C4_UI_ElementType_None,
    C4_UI_ElementType_Borders,
    C4_UI_ElementType_Button,
    C4_UI_ElementType_ButtonGroup,
    C4_UI_ElementType_NumberInput,
    C4_UI_ElementType_Popup,
    C4_UI_ElementType_Rectangle,
    C4_UI_ElementType_Symbol,
    C4_UI_ElementType_Text
} C4_UI_ElementType;

typedef struct {
    void* data;
    C4_UI_ElementType type;
    void (*Draw)(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY);
    bool (*HandleEvents)(void* data, SDL_Event* event, float scale, float parentX, float parentY);
    void (*Destroy)(void* data);
    void (*Reset)(void* data);
    void (*Update)(void* data, float deltaTime);
    bool (*HandleAction)(void* data, C4_InputAction action);
} C4_UI_Element;

typedef struct C4_UI_Node {
    C4_UI_Element element;
    struct C4_UI_Node* next;

    struct C4_UI_Node* navUp;
    struct C4_UI_Node* navDown;
    struct C4_UI_Node* navLeft;
    struct C4_UI_Node* navRight;

    bool isFocusable;
} C4_UI_Node;

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    SDL_Renderer* renderer;

    // [Owns] properties specific to this struct
    float destinationX;
    float destinationY;
    C4_UI_Node* head;
    C4_UI_Node* tail;

    // Every button's colors are reset when a popup is initally opened
    bool buttonColorResetComplete;

    C4_UI_Node* focusedNode;
} C4_UI_Canvas;

void C4_UI_Canvas_Init(C4_UI_Canvas* canvas, SDL_Renderer* renderer, float destinationX, float destinationY);
void C4_UI_Canvas_Destroy(C4_UI_Canvas* canvas);
void C4_UI_Canvas_Clear(C4_UI_Canvas* canvas);
void C4_UI_Canvas_Draw(C4_UI_Canvas* canvas, float scale);
void C4_UI_Canvas_Update(C4_UI_Canvas* canvas, float deltaTime);
void C4_UI_Canvas_HandleEvent(C4_UI_Canvas* canvas, SDL_Event* event, float scale);
void C4_UI_Canvas_ResetButtons(C4_UI_Canvas* canvas);
C4_UI_Node* C4_UI_Canvas_GetLastNode(C4_UI_Canvas* canvas);

C4_UI_Borders* C4_UI_Canvas_Add_Borders(
    C4_UI_Canvas* canvas, const C4_UI_Borders_Config* config
);
C4_UI_Button* C4_UI_Canvas_Add_Button(
    C4_UI_Canvas* canvas, const C4_UI_Button_Config* config
);
C4_UI_ButtonGroup* C4_UI_Canvas_Add_ButtonGroup(
    C4_UI_Canvas* canvas, const C4_UI_ButtonGroup_Config* config
);
C4_UI_NumberInput* C4_UI_Canvas_Add_NumberInput(
    C4_UI_Canvas* canvas, const C4_UI_NumberInput_Config* config
);
C4_UI_Popup* C4_UI_Canvas_Add_Popup(
    C4_UI_Canvas* canvas, const C4_UI_Popup_Config* config
);
C4_UI_Rectangle* C4_UI_Canvas_Add_Rectangle(
    C4_UI_Canvas* canvas, const C4_UI_Rectangle_Config* config
);
C4_UI_Symbol* C4_UI_Canvas_Add_Symbol(
    C4_UI_Canvas* canvas, const C4_UI_Symbol_Config* config
);
C4_UI_Text* C4_UI_Canvas_Add_Text(
    C4_UI_Canvas* canvas, const C4_UI_Text_Config* config
);