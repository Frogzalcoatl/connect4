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
    void (*Draw)(void* data, SDL_Renderer* renderer);
    void (*Update)(void* data, SDL_Event* event);
    void (*Destroy)(void* data);
} C4_UI_Element;

typedef struct C4_UI_Node {
    C4_UI_Element element;
    struct C4_UI_Node* next;
} C4_UI_Node;

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    SDL_Renderer* renderer;

    // [Owns] properties specific to this struct
    //SDL_FRect bounds;
    C4_UI_Node* head;
    C4_UI_Node* tail;
} C4_UI_Container;

void C4_UI_Container_Init(C4_UI_Container* container, SDL_Renderer* renderer);
void C4_UI_Container_Destroy(C4_UI_Container* container);
void C4_UI_Container_Clear(C4_UI_Container* container);
void C4_UI_Container_Draw(C4_UI_Container* container);
void C4_UI_Container_Update(C4_UI_Container* container, SDL_Event* event);

C4_UI_Borders* C4_UI_Container_Add_Borders(
    C4_UI_Container* container, const SDL_FRect destination,
    const SDL_Color color, const unsigned int width
);
C4_UI_Button* C4_UI_Container_Add_Button(
    C4_UI_Container* container, const SDL_FRect destination,
    const char* str, const C4_UI_Theme* theme
);
C4_UI_ButtonGroup* C4_UI_Container_Add_ButtonGroup(
    C4_UI_Container* container, const SDL_FRect bounds, size_t count,
    C4_UI_ButtonGroup_Direction direction, unsigned int margin, const C4_UI_Theme* theme
);
C4_UI_NumberInput* C4_UI_Container_Add_NumberInput(
    C4_UI_Container* container, const SDL_FRect destination, unsigned int min, unsigned int max,
    unsigned int startingValue, float arrowWidth, float arrowHeight, const C4_UI_Theme* theme
);
C4_UI_Popup* C4_UI_Container_Add_Popup(
    C4_UI_Container* container, const SDL_FRect destination, C4_UI_ButtonGroup_Direction buttonDirection,
    size_t buttonCount, float buttonGroupHeight, const char* messageText, const C4_UI_Theme* theme
);
C4_UI_Rectangle* C4_UI_Container_Add_Rectangle(
    C4_UI_Container* container, const SDL_FRect destination, const SDL_Color color 
);
C4_UI_Symbol* C4_UI_Container_Add_Symbol(
    C4_UI_Container* container, C4_UI_SymbolType type,
    const SDL_FRect destination, int rotationDegrees, const SDL_Color color
);
C4_UI_Text* C4_UI_Container_Add_Text(
    C4_UI_Container* container, const char* str, C4_FontType font, float ptSize,
    float destinationX, float destinationY, int wrapWidth, SDL_Color color
);