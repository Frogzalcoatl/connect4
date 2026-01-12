#include "Connect4/ui/canvas.h"
#include "Connect4/ui/cursorStyle.h"
#include <stdlib.h>

void C4_UI_Canvas_Init(C4_UI_Canvas* canvas, SDL_Renderer* renderer, TTF_TextEngine* textEngine, float offsetX, float offsetY) {
    if (!canvas || !textEngine) {
        SDL_Log("Unable to init canvas. One or more required pointers are NULL");
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

    // Using an arena to prevent memory fragmentation
    // Also runs faster this way
    // Also easier memory management
    // Also clearing the canvas is much more efficient (just setting offset to 0)
    size_t arenaSize = 1024 * 1024; // 1 MB
    canvas->arena.memory = malloc(arenaSize);
    canvas->arena.capacity = arenaSize;
    canvas->arena.offset = 0;
}

static void C4_UI_Canvas_CleanupResources(C4_UI_Canvas* canvas) {
    if (!canvas) {
        SDL_Log("Unable to cleanup canvas resources. Canvas is NULL");
        return;
    }

    C4_UI_Node* current = canvas->root;
    while (current) {
        C4_UI_Node_CleanupResources(current);
        current = current->nextSibling;
    }
}

void C4_UI_Canvas_Clear(C4_UI_Canvas* canvas) {
    if (!canvas) {
        SDL_Log("Unable to clear canvas. Canvas is NULL");
        return;
    }
    C4_UI_Canvas_CleanupResources(canvas);
    canvas->arena.offset = 0;
    canvas->root = NULL;
    canvas->focusedNode = NULL;
}

void C4_UI_Canvas_Destroy(C4_UI_Canvas* canvas) {
    if (!canvas) {
        return;
    }
    C4_UI_Canvas_CleanupResources(canvas);
    if (canvas->arena.memory) {
        free(canvas->arena.memory);
    }
}

void C4_UI_Canvas_Draw(C4_UI_Canvas* canvas, float scale) {
    if (!canvas || !canvas->renderer) {
        SDL_Log("Unable to draw canvas. One or more required pointers are NULL");
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
        SDL_Log("Unable to set canvas focus. Canvas is NULL");
        return;
    }
    if (canvas->focusedNode) {
        C4_UI_Interaction_Reset(&canvas->focusedNode->input);
    }

    if (!newNode || !newNode->input.isFocusable) {
        return;
    }

    newNode->input.isHovered = true;
    if (newNode->input.OnHover) {
        newNode->input.OnHover(newNode->input.context);
    }
    C4_PlaySound(newNode->input.sounds.onHover);
    canvas->focusedNode = newNode;
}

static bool C4_UI_Canvas_HandleAction(C4_UI_Canvas* canvas, C4_InputEvent event) {
    if (!canvas) {
        SDL_Log("Unable to handle actions through canvas. Canvas is NULL");
        return false;
    }

    C4_UI_Node* current = canvas->focusedNode;
    
    while (current) {
        if (C4_UI_Interaction_HandleAction(&current->input, event)) {
            return true;
        }
        current = current->parent;
    }

    if (event.state != C4_INPUT_STATE_PRESSED) {
        return false;
    }

    if (!canvas->focusedNode) {
        C4_UI_Node* current = canvas->root;
        while (current) {
            C4_UI_Node* result = C4_UI_Node_FindFocusable(current);
            if (result) {
                C4_UI_Canvas_SetFocus(canvas, result);
                if (
                    event.verb == C4_INPUT_VERB_CANCEL &&
                    event.state == C4_INPUT_STATE_PRESSED
                ) {
                    if (C4_UI_Canvas_HandleAction(canvas, event)) {
                        return true;
                    }
                }
                return false;
            }
            current = current->nextSibling;
        }
        return false;
    }

    C4_UI_Node* focusedNode = canvas->focusedNode;
    switch (event.verb) {
        case C4_INPUT_VERB_NAV_UP: {
            if (focusedNode->navUp) {
                C4_UI_Canvas_SetFocus(canvas, focusedNode->navUp);
                return true;
            }
        }; break;
        case C4_INPUT_VERB_NAV_DOWN: {
            if (focusedNode->navDown) {
                C4_UI_Canvas_SetFocus(canvas, focusedNode->navDown);
                return true;
            }
        }; break;
        case C4_INPUT_VERB_NAV_RIGHT: {
            if (focusedNode->navRight) {
                C4_UI_Canvas_SetFocus(canvas, focusedNode->navRight);
                return true;
            }
        }; break;
        case C4_INPUT_VERB_NAV_LEFT: {
            if (focusedNode->navLeft) {
                C4_UI_Canvas_SetFocus(canvas, focusedNode->navLeft);
                return true;
            }
        }; break;
        default: break;
    }
    return false;
}

static void C4_UI_Canvas_HandleMouseEvents(C4_UI_Canvas* canvas, SDL_Event* event) {
    if (!canvas || !event) {
        SDL_Log("Unable to handle mouse events in canvas. One or more required pointers are NULL");
        return;
    }

    if (
        event->type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
        event->button.button == SDL_BUTTON_X1
    ) {
        C4_InputEvent inputEvent = (C4_InputEvent){
            .verb = C4_INPUT_VERB_CANCEL,
            .state = C4_INPUT_STATE_PRESSED
        };
        if (C4_UI_Canvas_HandleAction(canvas, inputEvent)) {
            return;
        }
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
        SDL_Log("Unable to handle events in canvas. One or more required pointers are NULL");
        return;
    }
    (void)scale;
    C4_InputEvent inputEvent = C4_GetInput(event);

    if (inputEvent.verb != C4_INPUT_VERB_NONE) {
        if (SDL_CursorVisible()) {
            SDL_HideCursor();
            C4_UI_Canvas_ResetInteractions(canvas);
        }
        C4_UI_Canvas_HandleAction(canvas, inputEvent);
        return;
    } else if (
        event->type == SDL_EVENT_MOUSE_MOTION ||
        event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || 
        event->type == SDL_EVENT_MOUSE_BUTTON_UP ||
        event->type == SDL_EVENT_FINGER_UP // To properly reset isHovered on touch
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
        SDL_Log("Unable to update canvas. One or more required pointers are NULL");
        return;
    }
    if (deltaTime <= 0.f) {
        deltaTime = 0.0001f;
    }

    C4_InputEvent repeatEvent;
    while (C4_Input_CheckRepeat(deltaTime, &repeatEvent)) {
        C4_UI_Canvas_HandleAction(canvas, repeatEvent);
    }

    C4_UI_Node* current = canvas->root;
    while (current) {
        C4_UI_Node_Update(current, deltaTime);
        current = current->nextSibling;
    }
}

void C4_UI_Canvas_ResetInteractions(C4_UI_Canvas* canvas) {
    if (!canvas) {
        SDL_Log("Unable to reset canvas interactions. Canvas is NULL");
        return;
    }
    C4_UI_Node* current = canvas->root;
    while (current) {
        C4_UI_Node_Reset(current);
        current = current->nextSibling;
    }
}

void C4_UI_Canvas_AddNode(C4_UI_Canvas* canvas, C4_UI_Node* node) {
    if (!canvas || !node) {
        SDL_Log("Unable to add node to canvas. One or more required pointers are NULL");
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