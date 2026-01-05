#include "Connect4/ui/container.h"
#include "Connect4/ui/cursorManager.h"
#include <stdlib.h>

void C4_UI_Container_Init(C4_UI_Container* container, SDL_Renderer* renderer, float destinationX, float destinationY) {
    if (!container) {
        return;
    }
    container->head = NULL;
    container->tail = NULL;
    if (!renderer) {
        SDL_Log("Warning: Container renderer is NULL");
    }
    container->renderer = renderer;
    container->buttonColorResetComplete = true;
    container->destinationX = destinationX;
    container->destinationY = destinationY;
}

void C4_UI_Container_Clear(C4_UI_Container* container) {
    if (!container) {
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
    if (!container) {
        return;
    }
    C4_UI_Container_Clear(container);
    free(container);
}

void C4_UI_Container_Draw(C4_UI_Container* container, float scale) {
    if (!container || !container->renderer) {
        return;
    }
    C4_UI_Node* current = container->head;
    while (current != NULL) {
        C4_UI_Node* next = current->next;
        if (current->element.Draw && current->element.type != C4_UI_ElementType_Popup) {
            current->element.Draw(current->element.data, container->renderer, scale, container->destinationX, container->destinationY);
        }
        current = next;
    }
    // So popups are always drawn on top of everything else
    current = container->head;
    while (current != NULL) {
        if (current->element.type == C4_UI_ElementType_Popup) {
            if (current->element.Draw) {
                current->element.Draw(current->element.data, container->renderer, scale, container->destinationX, container->destinationY);
            }
        }
        current = current->next;
    }
}

void C4_UI_Container_HandleEvent(C4_UI_Container* container, SDL_Event* event, float scale) {
    if (!container || !event) {
        return;
    }
    bool isPopupActive = false;
    C4_UI_Node* current = container->head;
    while (current) {
        if (current->element.type == C4_UI_ElementType_Popup) {
            C4_UI_Popup* popup = (C4_UI_Popup*)current->element.data;
            if (popup->isShowing) {
                isPopupActive = true;
                current->element.HandleEvents(current->element.data, event, scale, container->destinationX, container->destinationY);
            }
        }
        current = current->next;
    }
    if (!isPopupActive) {
        current = container->head;
        while (current) {
            if (current->element.type != C4_UI_ElementType_Popup) {
                if (current->element.HandleEvents) {
                    current->element.HandleEvents(current->element.data, event, scale, container->destinationX, container->destinationY);
                }
            }
            current = current->next;
        }
    }
}

void C4_UI_Container_Update(C4_UI_Container* container, float deltaTime) {
    if (!container) {
        return;
    }
    if (deltaTime <= 0.f) {
        deltaTime = 0.0001f;
    }
    bool isPopupShowing = false;
    C4_UI_Node* current = container->head;
    while (current) {
        if (current->element.type == C4_UI_ElementType_Popup) {
            C4_UI_Popup* p = (C4_UI_Popup*)current->element.data;
            if (p->isShowing) {
                isPopupShowing = true;
                break;
            }
        }
        current = current->next;
    }
    if (isPopupShowing) {
        if (!container->buttonColorResetComplete) {
            current = container->head;
            while (current) {
                if (current->element.Reset) {
                    current->element.Reset(current->element.data);
                }
                current = current->next;
            }
            container->buttonColorResetComplete = true;
            SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
        }
        return;
    }
    container->buttonColorResetComplete = false;
    current = container->head;
    while (current) {
        if (current->element.Update) {
            current->element.Update(current->element.data, deltaTime);
        }
        current = current->next;
    }
}

static void C4_UI_Container_AddNode(
    C4_UI_Container* container, void* data,
    C4_UI_ElementType type,
    void (*Draw)(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY),
    void (*HandleEvents)(void* data, SDL_Event* event, float scale, float parentX, float parentY),
    void (*Destroy)(void*),
    void (*Reset)(void*),
    void (*Update)(void*, float deltaTime)
) {
    // Some elements dont have update and/or reset functions
    if (
        !container || !data ||
        !Draw || !Destroy
    ) {
        return;
    }
    C4_UI_Node* newNode = calloc(1, sizeof(C4_UI_Node));
    if (!newNode) {
        SDL_Log("Unable to add Node to C4 Container");
        return;
    }
    newNode->element.data = data;
    newNode->element.type = type;
    newNode->element.Draw = Draw;
    newNode->element.HandleEvents = HandleEvents;
    newNode->element.Destroy = Destroy;
    newNode->element.Reset = Reset;
    newNode->element.Update = Update;
    newNode->next = NULL;
    if (!container->head) {
        container->head = newNode;
        container->tail = newNode;
    } else {
        container->tail->next = newNode;
        container->tail = newNode;
    }
}

C4_UI_Borders* C4_UI_Container_Add_Borders(
    C4_UI_Container* container, const C4_UI_Borders_Config* config
) {
    if (!container) {
        return NULL;
    }
    C4_UI_Borders* borders = C4_UI_Borders_Create(config);
    C4_UI_Container_AddNode(
        container, borders, C4_UI_ElementType_Borders,
        C4_UI_Borders_Draw, NULL, C4_UI_Borders_Destroy, NULL, NULL
    );
    return borders;
}

C4_UI_Button* C4_UI_Container_Add_Button(
    C4_UI_Container* container, const C4_UI_Button_Config* config
) {
    if (!container) {
        return NULL;
    }
    C4_UI_Button* button = C4_UI_Button_Create(container->renderer, config);
    C4_UI_Container_AddNode(
        container, button, C4_UI_ElementType_Button,
        C4_UI_Button_Draw, C4_UI_Button_HandleMouseEvents,
        C4_UI_Button_Destroy, C4_UI_Button_Reset, C4_UI_Button_Update
    );
    return button;
}

C4_UI_ButtonGroup* C4_UI_Container_Add_ButtonGroup(
    C4_UI_Container* container, const C4_UI_ButtonGroup_Config* config
) {
    if (!container) {
        return NULL;
    }
    C4_UI_ButtonGroup* buttonGroup = C4_UI_ButtonGroup_Create(container->renderer, config);
    C4_UI_Container_AddNode(
        container, buttonGroup, C4_UI_ElementType_ButtonGroup,
        C4_UI_ButtonGroup_Draw, C4_UI_ButtonGroup_HandleMouseEvents,
        C4_UI_ButtonGroup_Destroy, C4_UI_ButtonGroup_Reset, C4_UI_ButtonGroup_Update
    );
    return buttonGroup;
}

C4_UI_NumberInput* C4_UI_Container_Add_NumberInput(
    C4_UI_Container* container, const C4_UI_NumberInput_Config* config
) {
    if (!container) {
        return NULL;
    }
    C4_UI_NumberInput* numInput = C4_UI_NumberInput_Create(container->renderer, config);
    C4_UI_Container_AddNode(
        container, numInput, C4_UI_ElementType_NumberInput,
        C4_UI_NumberInput_Draw, C4_UI_NumberInput_HandleMouseEvents,
        C4_UI_NumberInput_Destroy, C4_UI_NumberInput_ResetButtons,
        C4_UI_NumberInput_Update
    );
    return numInput;
}

C4_UI_Popup* C4_UI_Container_Add_Popup(
    C4_UI_Container* container, const C4_UI_Popup_Config* config
) {
    if (!container) {
        return NULL;
    }
    C4_UI_Popup* popup = C4_UI_Popup_Create(
        container->renderer, config
    );
    C4_UI_Container_AddNode(
        container, popup, C4_UI_ElementType_Popup,
        C4_UI_Popup_Draw, C4_UI_Popup_HandleMouseEvents,
        C4_UI_Popup_Destroy, C4_UI_Popup_ResetButtons,
        C4_UI_Popup_Update
    );
    return popup;
}

C4_UI_Rectangle* C4_UI_Container_Add_Rectangle(
    C4_UI_Container* container, const C4_UI_Rectangle_Config* config
) {
    if (!container) {
        return NULL;
    }
    C4_UI_Rectangle* rectangle = C4_UI_Rectangle_Create(config);
    C4_UI_Container_AddNode(
        container, rectangle, C4_UI_ElementType_Rectangle,
        C4_UI_Rectangle_Draw, NULL, C4_UI_Rectangle_Destroy, NULL, NULL
    );
    return rectangle;
}

C4_UI_Symbol* C4_UI_Container_Add_Symbol(
    C4_UI_Container* container, const C4_UI_Symbol_Config* config
) {
    if (!container) {
        return NULL;
    }
    C4_UI_Symbol* symbol = C4_UI_Symbol_Create(config);
    C4_UI_Container_AddNode(
        container, symbol, C4_UI_ElementType_Symbol,
        C4_UI_Symbol_Draw, NULL, C4_UI_Symbol_Destroy, NULL, NULL
    );
    return symbol;
}

C4_UI_Text* C4_UI_Container_Add_Text(
    C4_UI_Container* container, const C4_UI_Text_Config* config
) {
    if (!container) {
        return NULL;
    }
    C4_UI_Text* text = C4_UI_Text_Create(
        container->renderer, config
    );
    C4_UI_Container_AddNode(
        container, text, C4_UI_ElementType_Text,
        C4_UI_Text_Draw, NULL, C4_UI_Text_Destroy, NULL, NULL
    );
    return text;
}