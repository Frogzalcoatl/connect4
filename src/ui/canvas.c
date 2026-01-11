#include "Connect4/ui/canvas.h"
#include "Connect4/ui/cursorManager.h"
#include <stdlib.h>

void C4_UI_Canvas_Init(C4_UI_Canvas* canvas, SDL_Renderer* renderer, TTF_TextEngine* textEngine, float offsetX, float offsetY) {
    if (!canvas || !textEngine) {
        return;
    }
    if (!renderer) {
        SDL_Log("Warning: Canvas renderer is NULL");
    }

    canvas->root = NULL;
    canvas->focusedNode = NULL;

    canvas->renderer = renderer;
    canvas->textEngine = textEngine;

    canvas->offsetX = offsetX;
    canvas->offsetY = offsetY;
}

void C4_UI_Canvas_Destroy(C4_UI_Canvas* canvas) {
    if (!canvas) {
        return;
    }
    C4_UI_Node_Destroy(canvas->root);
    canvas->root = NULL;
    canvas->focusedNode = NULL;
    free(canvas);
}

void C4_UI_Canvas_Draw(C4_UI_Canvas* canvas, float scale) {
    if (!canvas || !canvas->renderer) {
        return;
    }
    
    C4_UI_Node* current = canvas->root;
    while (current) {
        C4_UI_Node_Draw(current, canvas->renderer, scale, canvas->offsetX, canvas->offsetY);
        current = current->nextSibling;
    }
}

static void C4_UI_Canvas_SetFocus(C4_UI_Canvas* canvas, C4_UI_Node* newNode) {
    if (!canvas) {
        return;
    }
    if (canvas->focusedNode) {
        C4_UI_Interaction_Reset(&canvas->focusedNode->input);
    }

    if (newNode && newNode->input.isFocusable) {
        canvas->focusedNode = newNode;
        canvas->focusedNode->input.isHovered = true;
    }
}

static void C4_UI_Canvas_HandleAction(C4_UI_Canvas* canvas, C4_InputEvent event) {
    if (!canvas) {
        return;
    }

    C4_UI_Node* current = canvas->focusedNode;
    if (current) {
        if (C4_UI_Node_HandleAction(current, event)) {
            return;
        }
        current = current->nextSibling;
    }

    if (event.state != C4_INPUT_STATE_PRESSED) {
        return;
    }

    if (!canvas->focusedNode) {
        C4_UI_Node* current = canvas->root;
        while (current) {
            C4_UI_Node* result = C4_UI_Node_FindFocusable(current);
            if (result) {
                C4_UI_Canvas_SetFocus(canvas, result);
                SDL_Log("Found a focusable node");
                return;
            }
            current = current->nextSibling;
        }
        return;
    }

    C4_UI_Node* focusedNode = canvas->focusedNode;
    switch (event.verb) {
        case C4_INPUT_NAV_UP: {
            if (focusedNode->navUp) {
                C4_UI_Canvas_SetFocus(canvas, focusedNode->navUp);
            }
        }; break;
        case C4_INPUT_NAV_DOWN: {
            if (focusedNode->navDown) {
                C4_UI_Canvas_SetFocus(canvas, focusedNode->navDown);
            }
        }; break;
        case C4_INPUT_NAV_RIGHT: {
            if (focusedNode->navRight) {
                C4_UI_Canvas_SetFocus(canvas, focusedNode->navRight);
            }
        }; break;
        case C4_INPUT_NAV_LEFT: {
            if (focusedNode->navLeft) {
                C4_UI_Canvas_SetFocus(canvas, focusedNode->navLeft);
            }
        }; break;
        default: break;
    }
}

static void C4_UI_Canvas_HandleMouseEvents(C4_UI_Canvas* canvas, SDL_Event* event) {
    if (!canvas || !event) {
        return;
    }
    
    C4_UI_Node* current = canvas->root;
    while (current) {
        if (C4_UI_Node_HandleMouseEvents(current, event)) {
            return;
        }
        current = current->nextSibling;
    }
}

void C4_UI_Canvas_HandleEvent(C4_UI_Canvas* canvas, SDL_Event* event, float scale) {
    if (!canvas || !event) {
        return;
    }
    (void)scale;
    C4_InputEvent inputEvent = C4_GetInput(event);

    if (inputEvent.verb != C4_INPUT_NONE) {
        if (SDL_CursorVisible()) {
            SDL_HideCursor();
            C4_UI_Canvas_ResetInteractions(canvas);
        }
        C4_UI_Canvas_HandleAction(canvas, inputEvent);
        return;
    } else if (
        event->type == SDL_EVENT_MOUSE_MOTION ||
        event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || 
        event->type == SDL_EVENT_MOUSE_BUTTON_UP
    ) {
        if (!SDL_CursorVisible()) {
            SDL_ShowCursor();
            C4_UI_Canvas_ResetInteractions(canvas);
        }

        // So theres not two nodes focused at the same time from controller and mouse
        if (event->type == SDL_EVENT_MOUSE_MOTION) {
            if (canvas->focusedNode) {
                canvas->focusedNode->input.isHovered = false;
            }
            canvas->focusedNode = NULL;
        }

        C4_UI_Canvas_HandleMouseEvents(canvas, event);
    }
}

void C4_UI_Canvas_Update(C4_UI_Canvas* canvas, float deltaTime) {
    if (!canvas) {
        return;
    }
    if (deltaTime <= 0.f) {
        deltaTime = 0.0001f;
    }
}

static void C4_UI_Node_Reset(C4_UI_Node* node) {
    if (!node) {
        return;
    }

    C4_UI_Interaction_Reset(&node->input);

    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node_Reset(child);
        child = child->nextSibling;
    }
}

void C4_UI_Canvas_ResetInteractions(C4_UI_Canvas* canvas) {
    if (!canvas) {
        return;
    }
    C4_UI_Node* current = canvas->root;
    while (current) {
        C4_UI_Node_Reset(current);
        current = current->nextSibling;
    }
}

void C4_UI_Canvas_AddNode(C4_UI_Canvas* canvas, C4_UI_Node* node) {
    if (!canvas) {
        return;
    }
    if (!canvas->root) {
        canvas->root = node;
        node->prevSibling = NULL;
        node->nextSibling = NULL;
    } else {
        C4_UI_Node_PushNode(canvas->root, node);
    }
}