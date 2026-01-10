#include "Connect4/ui/canvas.h"
#include "Connect4/ui/cursorManager.h"
#include <stdlib.h>

void C4_UI_Canvas_Init(C4_UI_Canvas* canvas, SDL_Renderer* renderer, float destinationX, float destinationY) {
    if (!canvas) {
        return;
    }
    canvas->head = NULL;
    canvas->tail = NULL;
    if (!renderer) {
        SDL_Log("Warning: Canvas renderer is NULL");
    }
    canvas->renderer = renderer;
    canvas->buttonColorResetComplete = true;
    canvas->destinationX = destinationX;
    canvas->destinationY = destinationY;
    canvas->focusedNode = NULL;
}

void C4_UI_Canvas_Clear(C4_UI_Canvas* canvas) {
    if (!canvas) {
        return;
    }
    C4_UI_Node* current = canvas->head;
    while (current != NULL) {
        C4_UI_Node* next = current->next;
        if (current->element.Destroy) {
            current->element.Destroy(current->element.data);
        }
        free(current);
        current = next;
    }
    canvas->head = NULL;
    canvas->tail = NULL;
}

void C4_UI_Canvas_Destroy(C4_UI_Canvas* canvas) {
    if (!canvas) {
        return;
    }
    C4_UI_Canvas_Clear(canvas);
    free(canvas);
}

void C4_UI_Canvas_Draw(C4_UI_Canvas* canvas, float scale) {
    if (!canvas || !canvas->renderer) {
        return;
    }
    C4_UI_Node* current = canvas->head;
    while (current != NULL) {
        C4_UI_Node* next = current->next;
        if (current->element.Draw && current->element.type != C4_UI_ElementType_Popup) {
            current->element.Draw(current->element.data, canvas->renderer, scale, canvas->destinationX, canvas->destinationY);
        }
        current = next;
    }
    // So popups are always drawn on top of everything else
    current = canvas->head;
    while (current != NULL) {
        if (current->element.type == C4_UI_ElementType_Popup) {
            if (current->element.Draw) {
                current->element.Draw(current->element.data, canvas->renderer, scale, canvas->destinationX, canvas->destinationY);
            }
        }
        current = current->next;
    }
}

static void C4_UI_SetFocus(C4_UI_Canvas* canvas, C4_UI_Node* newNode) {
    if (canvas->focusedNode->element.type == C4_UI_ElementType_Button) {
        C4_UI_Button* oldButton = (C4_UI_Button*)canvas->focusedNode->element.data;
        oldButton->isFocused = false;
    }

    canvas->focusedNode = newNode;
    if (newNode && newNode->isFocusable) {
        if (newNode->element.type == C4_UI_ElementType_Button) {
            C4_UI_Button* newBtn = (C4_UI_Button*)newNode->element.data;
            newBtn->isFocused = true;
        }
    }
}

static void C4_UI_Canvas_HandleAction(C4_UI_Canvas* canvas, C4_InputAction action) {
    if (!canvas || !canvas->head) {
        return;
    }

    if (!canvas->focusedNode) {
        C4_UI_Node* current = canvas->head;
        while (current) {
            if (current->isFocusable) {
                C4_UI_SetFocus(canvas, current);
                SDL_Log("Found a focusable node");
                return;
            }
            current = current->next;
        }
        return;
    }

    C4_UI_Node* current = canvas->focusedNode;
    if (current->element.HandleAction) {
        if (current->element.HandleAction(current->element.data, action)) {
            return;
        }
    }

    switch (action) {
        case C4_ACTION_NAV_UP: {
            if (current->navUp) {
                C4_UI_SetFocus(canvas, current->navUp);
            }
        }; break;
        case C4_ACTION_NAV_DOWN: {
            if (current->navDown) {
                C4_UI_SetFocus(canvas, current->navDown);
            }
        }; break;
        case C4_ACTION_NAV_RIGHT: {
            if (current->navRight) {
                C4_UI_SetFocus(canvas, current->navRight);
            }
        }; break;
        case C4_ACTION_NAV_LEFT: {
            if (current->navLeft) {
                C4_UI_SetFocus(canvas, current->navLeft);
            }
        }; break;
    }
}

void C4_UI_Canvas_HandleEvent(C4_UI_Canvas* canvas, SDL_Event* event, float scale) {
    if (!canvas || !event) {
        return;
    }

    C4_InputAction action = C4_GetInput(event);

    if (action != C4_ACTION_NONE) {
        if (SDL_CursorVisible()) {
            SDL_HideCursor();
        }
        C4_UI_Canvas_HandleAction(canvas, action);
        return;
    } else if (
        event->type == SDL_EVENT_MOUSE_MOTION ||
        event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || 
        event->type == SDL_EVENT_MOUSE_BUTTON_UP
    ) {
        if (!SDL_CursorVisible()) {
            SDL_ShowCursor();
        }
        // So theres not two nodes focused at once from controller and mouse
        if (event->type == SDL_EVENT_MOUSE_MOTION) {
            if (canvas->focusedNode && canvas->focusedNode->element.type == C4_UI_ElementType_Button) {
                C4_UI_Button* button = (C4_UI_Button*)canvas->focusedNode->element.data;
                button->isFocused = false;
            }
            canvas->focusedNode = NULL;
        }

        C4_UI_Node* current = canvas->head;
        while (current) {
            if (current->element.HandleEvents) {
                if (current->element.HandleEvents(current->element.data, event, scale, canvas->destinationX, canvas->destinationY)) {
                    return;
                }
            }
            current = current->next;
        }
    }
}

void C4_UI_Canvas_Update(C4_UI_Canvas* canvas, float deltaTime) {
    if (!canvas) {
        return;
    }
    if (deltaTime <= 0.f) {
        deltaTime = 0.0001f;
    }
    bool isPopupShowing = false;
    C4_UI_Node* current = canvas->head;
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
        if (!canvas->buttonColorResetComplete) {
            current = canvas->head;
            while (current) {
                if (current->element.Reset) {
                    current->element.Reset(current->element.data);
                }
                current = current->next;
            }
            canvas->buttonColorResetComplete = true;
            SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
        }
        return;
    }
    canvas->buttonColorResetComplete = false;
    current = canvas->head;
    while (current) {
        if (current->element.Update) {
            current->element.Update(current->element.data, deltaTime);
        }
        current = current->next;
    }
}

void C4_UI_Canvas_ResetButtons(C4_UI_Canvas* canvas) {
    if (!canvas) {
        return;
    }
    C4_UI_Node* current = canvas->head;
    while (current) {
        if (current->element.Reset) {
            current->element.Reset(current->element.data);
        }
        current = current->next;
    }
}

C4_UI_Node* C4_UI_Canvas_GetLastNode(C4_UI_Canvas* canvas) {
    if (!canvas) {
        return NULL;
    }
    return canvas->tail;
}

static void C4_UI_Canvas_AddNode(
    C4_UI_Canvas* canvas, void* data,
    C4_UI_ElementType type,
    void (*Draw)(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY),
    bool (*HandleEvents)(void* data, SDL_Event* event, float scale, float parentX, float parentY),
    void (*Destroy)(void*),
    void (*Reset)(void*),
    void (*Update)(void*, float deltaTime),
    bool (*HandleAction)(void* data, C4_InputAction action)
) {
    // All nodes should at least have a draw and destroy function
    if (
        !canvas || !data ||
        !Draw || !Destroy
    ) {
        return;
    }
    C4_UI_Node* newNode = calloc(1, sizeof(C4_UI_Node));
    if (!newNode) {
        SDL_Log("Unable to add Node to C4 Canvas");
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

    if (newNode->element.type == C4_UI_ElementType_Button) {
        newNode->isFocusable = true;
    } else {
        newNode->isFocusable = false;
    }

    if (!canvas->head) {
        canvas->head = newNode;
        canvas->tail = newNode;
    } else {
        canvas->tail->next = newNode;
        canvas->tail = newNode;
    }
}

C4_UI_Borders* C4_UI_Canvas_Add_Borders(
    C4_UI_Canvas* canvas, const C4_UI_Borders_Config* config
) {
    if (!canvas) {
        return NULL;
    }
    C4_UI_Borders* borders = C4_UI_Borders_Create(config);
    C4_UI_Canvas_AddNode(
        canvas, borders, C4_UI_ElementType_Borders,
        C4_UI_Borders_Draw, NULL, C4_UI_Borders_Destroy, NULL, NULL, NULL
    );
    return borders;
}

C4_UI_Button* C4_UI_Canvas_Add_Button(
    C4_UI_Canvas* canvas, const C4_UI_Button_Config* config
) {
    if (!canvas) {
        return NULL;
    }
    C4_UI_Button* button = C4_UI_Button_Create(canvas->renderer, config);
    C4_UI_Canvas_AddNode(
        canvas, button, C4_UI_ElementType_Button,
        C4_UI_Button_Draw, C4_UI_Button_HandleMouseEvents,
        C4_UI_Button_Destroy, C4_UI_Button_Reset, C4_UI_Button_Update,
        C4_UI_Button_HandleAction
    );
    return button;
}

C4_UI_ButtonGroup* C4_UI_Canvas_Add_ButtonGroup(
    C4_UI_Canvas* canvas, const C4_UI_ButtonGroup_Config* config
) {
    if (!canvas) {
        return NULL;
    }
    C4_UI_ButtonGroup* buttonGroup = C4_UI_ButtonGroup_Create(canvas->renderer, config);
    C4_UI_Canvas_AddNode(
        canvas, buttonGroup, C4_UI_ElementType_ButtonGroup,
        C4_UI_ButtonGroup_Draw, C4_UI_ButtonGroup_HandleMouseEvents,
        C4_UI_ButtonGroup_Destroy, C4_UI_ButtonGroup_Reset, C4_UI_ButtonGroup_Update, NULL
    );
    return buttonGroup;
}

C4_UI_NumberInput* C4_UI_Canvas_Add_NumberInput(
    C4_UI_Canvas* canvas, const C4_UI_NumberInput_Config* config
) {
    if (!canvas) {
        return NULL;
    }
    C4_UI_NumberInput* numInput = C4_UI_NumberInput_Create(canvas->renderer, config);
    C4_UI_Canvas_AddNode(
        canvas, numInput, C4_UI_ElementType_NumberInput,
        C4_UI_NumberInput_Draw, C4_UI_NumberInput_HandleMouseEvents,
        C4_UI_NumberInput_Destroy, C4_UI_NumberInput_ResetButtons,
        C4_UI_NumberInput_Update, NULL
    );
    return numInput;
}

C4_UI_Popup* C4_UI_Canvas_Add_Popup(
    C4_UI_Canvas* canvas, const C4_UI_Popup_Config* config
) {
    if (!canvas) {
        return NULL;
    }
    C4_UI_Popup* popup = C4_UI_Popup_Create(
        canvas->renderer, config
    );
    C4_UI_Canvas_AddNode(
        canvas, popup, C4_UI_ElementType_Popup,
        C4_UI_Popup_Draw, C4_UI_Popup_HandleMouseEvents,
        C4_UI_Popup_Destroy, C4_UI_Popup_ResetButtons,
        C4_UI_Popup_Update, NULL
    );
    return popup;
}

C4_UI_Rectangle* C4_UI_Canvas_Add_Rectangle(
    C4_UI_Canvas* canvas, const C4_UI_Rectangle_Config* config
) {
    if (!canvas) {
        return NULL;
    }
    C4_UI_Rectangle* rectangle = C4_UI_Rectangle_Create(config);
    C4_UI_Canvas_AddNode(
        canvas, rectangle, C4_UI_ElementType_Rectangle,
        C4_UI_Rectangle_Draw, NULL, C4_UI_Rectangle_Destroy, NULL, NULL, NULL
    );
    return rectangle;
}

C4_UI_Symbol* C4_UI_Canvas_Add_Symbol(
    C4_UI_Canvas* canvas, const C4_UI_Symbol_Config* config
) {
    if (!canvas) {
        return NULL;
    }
    C4_UI_Symbol* symbol = C4_UI_Symbol_Create(config);
    C4_UI_Canvas_AddNode(
        canvas, symbol, C4_UI_ElementType_Symbol,
        C4_UI_Symbol_Draw, NULL, C4_UI_Symbol_Destroy, NULL, NULL, NULL
    );
    return symbol;
}

C4_UI_Text* C4_UI_Canvas_Add_Text(
    C4_UI_Canvas* canvas, const C4_UI_Text_Config* config
) {
    if (!canvas) {
        return NULL;
    }
    C4_UI_Text* text = C4_UI_Text_Create(
        canvas->renderer, config
    );
    C4_UI_Canvas_AddNode(
        canvas, text, C4_UI_ElementType_Text,
        C4_UI_Text_Draw, NULL, C4_UI_Text_Destroy, NULL, NULL, NULL
    );
    return text;
}