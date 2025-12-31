#include "Connect4/ui/container.h"
#include <stdlib.h>

void C4_UI_Container_Init(C4_UI_Container* container, SDL_Renderer* renderer) {
    if (container == NULL) {
        return;
    }
    container->head = NULL;
    container->tail = NULL;
    if (renderer == NULL) {
        SDL_Log("Warning: Container renderer is NULL");
    }
    container->renderer = renderer;
}

void C4_UI_Container_Clear(C4_UI_Container* container) {
    if (container == NULL) {
        return;
    }
    C4_UI_Node* current = container->head;
    while (current != NULL) {
        C4_UI_Node* next = current->next;
        if (current->element.Destroy) {
            current->element.Destroy(current->element.data);
        }
        free(current);
        current = next;
    }
    container->head = NULL;
    container->tail = NULL;
}

void C4_UI_Container_Destroy(C4_UI_Container* container) {
    if (container == NULL) {
        return;
    }
    C4_UI_Container_Clear(container);
    free(container);
}

void C4_UI_Container_Draw(C4_UI_Container* container) {
    if (container == NULL || container->renderer == NULL) {
        return;
    }
    C4_UI_Node* current = container->head;
    while (current != NULL) {
        C4_UI_Node* next = current->next;
        if (current->element.Draw) {
            current->element.Draw(current->element.data, container->renderer);
        }
        current = next;
    }
}

void C4_UI_Container_Update(C4_UI_Container* container, SDL_Event* event) {
    if (container == NULL || event == NULL) {
        return;
    }
    C4_UI_Node* current = container->head;
    while (current != NULL) {
        C4_UI_Node* next = current->next;
        if (current->element.Update) {
            current->element.Update(current->element.data, event);
        }
        current = next;
    }
}

static void C4_UI_Container_AddNode(
    C4_UI_Container* container, void* data,
    C4_UI_ElementType type,
    void (*Draw)(void*, SDL_Renderer*),
    void (*Update)(void*, SDL_Event*),
    void (*Destroy)(void*)
) {
    // Some elements dont have an update function
    if (
        container == NULL || data == NULL ||
        Draw == NULL || Destroy == NULL
    ) {
        return;
    }
    C4_UI_Node* newNode = calloc(1, sizeof(C4_UI_Node));
    if (newNode == NULL) {
        SDL_Log("Unable to add Node to C4 Container");
        return;
    }
    newNode->element.data = data;
    newNode->element.type = type;
    newNode->element.Draw = Draw;
    newNode->element.Update = Update;
    newNode->element.Destroy = Destroy;
    newNode->next = NULL;
    if (container->head == NULL) {
        container->head = newNode;
        container->tail = newNode;
    } else {
        container->tail->next = newNode;
        container->tail = newNode;
    }
}

C4_UI_Borders* C4_UI_Container_Add_Borders(
    C4_UI_Container* container, const SDL_FRect destination,
    const SDL_Color color, const unsigned int width
) {
    if (container == NULL) {
        return NULL;
    }
    C4_UI_Borders* borders = C4_UI_Borders_Create(destination, color, width);
    C4_UI_Container_AddNode(
        container, borders, C4_UI_ElementType_Borders,
        C4_UI_Borders_Draw, NULL, C4_UI_Borders_Destroy
    );
    return borders;
}

C4_UI_Button* C4_UI_Container_Add_Button(
    C4_UI_Container* container, const SDL_FRect destination,
    const char* str, const C4_UI_Theme* theme
) {
    if (container == NULL) {
        return NULL;
    }
    C4_UI_Button* button = C4_UI_Button_Create(
        container->renderer, str, destination,
        C4_UI_SymbolType_None, 0.f, 0.f, 0, theme
    );
    C4_UI_Container_AddNode(
        container, button, C4_UI_ElementType_Button,
        C4_UI_Button_Draw, C4_UI_Button_HandleMouseEvents, C4_UI_Button_Destroy
    );
    return button;
}

C4_UI_ButtonGroup* C4_UI_Container_Add_ButtonGroup(
    C4_UI_Container* container, const SDL_FRect bounds, size_t count,
    C4_UI_ButtonGroup_Direction direction, unsigned int margin, const C4_UI_Theme* theme
) {
    if (container == NULL) {
        return NULL;
    }
    C4_UI_ButtonGroup* buttonGroup = C4_UI_ButtonGroup_Create(
        container->renderer, bounds, count, direction, margin, theme
    );
    C4_UI_Container_AddNode(
        container, buttonGroup, C4_UI_ElementType_ButtonGroup,
        C4_UI_ButtonGroup_Draw, C4_UI_ButtonGroup_HandleMouseEvents, C4_UI_ButtonGroup_Destroy
    );
    return buttonGroup;
}

C4_UI_NumberInput* C4_UI_Container_Add_NumberInput(
    C4_UI_Container* container, const SDL_FRect destination, unsigned int min, unsigned int max,
    unsigned int startingValue, float arrowWidth, float arrowHeight, const C4_UI_Theme* theme
) {
    if (container == NULL) {
        return NULL;
    }
    C4_UI_NumberInput* numInput = C4_UI_NumberInput_Create(
        container->renderer, destination, min, max, startingValue,
        arrowWidth, arrowHeight, theme
    );
    C4_UI_Container_AddNode(
        container, numInput, C4_UI_ElementType_NumberInput,
        C4_UI_NumberInput_Draw, C4_UI_NumberInput_HandleMouseEvents, C4_UI_NumberInput_Destroy
    );
    return numInput;
}

C4_UI_Popup* C4_UI_Container_Add_Popup(
    C4_UI_Container* container, const SDL_FRect destination, C4_UI_ButtonGroup_Direction buttonDirection,
    size_t buttonCount, float buttonGroupHeight, const char* messageText, const C4_UI_Theme* theme
) {
    if (container == NULL) {
        return NULL;
    }
    C4_UI_Popup* popup = C4_UI_Popup_Create(
        container->renderer, destination, buttonDirection,
        buttonCount, buttonGroupHeight, messageText, theme
    );
    C4_UI_Container_AddNode(
        container, popup, C4_UI_ElementType_Popup,
        C4_UI_Popup_Draw, C4_UI_Popup_HandleMouseEvents, C4_UI_Popup_Destroy
    );
    return popup;
}

C4_UI_Rectangle* C4_UI_Container_Add_Rectangle(
    C4_UI_Container* container, const SDL_FRect destination, const SDL_Color color 
) {
    if (container == NULL) {
        return NULL;
    }
    C4_UI_Rectangle* rectangle = C4_UI_Rectangle_Create(destination, color);
    C4_UI_Container_AddNode(
        container, rectangle, C4_UI_ElementType_Rectangle,
        C4_UI_Rectangle_Draw, NULL, C4_UI_Rectangle_Destroy
    );
    return rectangle;
}

C4_UI_Symbol* C4_UI_Container_Add_Symbol(
    C4_UI_Container* container, C4_UI_SymbolType type,
    const SDL_FRect destination, int rotationDegrees, const SDL_Color color
) {
    if (container == NULL) {
        return NULL;
    }
    C4_UI_Symbol* symbol = C4_UI_Symbol_Create(type, destination, rotationDegrees, color);
    C4_UI_Container_AddNode(
        container, symbol, C4_UI_ElementType_Symbol,
        C4_UI_Symbol_Draw, NULL, C4_UI_Symbol_Destroy
    );
    return symbol;
}

C4_UI_Text* C4_UI_Container_Add_Text(
    C4_UI_Container* container, const char* str, C4_FontType font, float ptSize,
    float destinationX, float destinationY, int wrapWidth, SDL_Color color
) {
    if (container == NULL) {
        return NULL;
    }
    C4_UI_Text* text = C4_UI_Text_Create(
        container->renderer, str, font, ptSize,
        destinationX, destinationY, wrapWidth, color
    );
    C4_UI_Container_AddNode(
        container, text, C4_UI_ElementType_Text,
        C4_UI_Text_Draw, NULL, C4_UI_Text_Destroy
    );
    return text;
}