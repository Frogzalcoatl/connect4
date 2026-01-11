#include "Connect4/ui/node.h"
#include "Connect4/ui/draw/shapes.h"
#include "Connect4/ui/draw/text.h"
#include <stdlib.h>

void C4_UI_Node_Destroy(C4_UI_Node* node) {
    if (!node) {
        return;
    }
    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node* next = child->nextSibling;
        C4_UI_Node_Destroy(child);
        child = next;
    }
    if (node->type == C4_UI_Type_Text && node->text.textObject) {
        TTF_DestroyText(node->text.textObject);
    }
    free(node);
}

static void C4_UI_Node_CalculateAbsoluteRect(C4_UI_Node* node, float scale, float parentX, float parentY) {
    node->absoluteRect.x = parentX + (node->rect.x * scale);
    node->absoluteRect.y = parentY + (node->rect.y * scale);
    node->absoluteRect.w = node->rect.w * scale;
    node->absoluteRect.h = node->rect.h * scale;
}

void C4_UI_Node_Draw(C4_UI_Node* node, SDL_Renderer* renderer, float scale, float parentX, float parentY) {
    if (!node) {
        return;
    }

    C4_UI_Interaction* activeInput = &node->input;
    
    if (node->inheritState && node->parent) {
        activeInput = &node->parent->input;
    }

    C4_UI_StyleState* currentStyle = &node->style.normal;
    if (activeInput->isPressed) {
        currentStyle = &node->style.pressed;
    } else if (activeInput->isHovered) {
        currentStyle = &node->style.hovered;
    } else if (!activeInput->isActive) {
        currentStyle = &node->style.inactive;
    }

    C4_UI_Node_CalculateAbsoluteRect(node, scale, parentX, parentY);

    switch (node->type) {
        case C4_UI_Type_Shape: {
            C4_UI_DrawShape(node->absoluteRect, &node->shape, currentStyle, renderer, scale);
        }; break;
        case C4_UI_Type_Text: {
            C4_UI_DrawText(node->absoluteRect, &node->text, currentStyle);
        }; break;
        default: break;
    }

    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node_Draw(child, renderer, scale, node->absoluteRect.x, node->absoluteRect.y);
        child = child->nextSibling;
    }
}

C4_UI_Node* C4_UI_Node_FindFocusable(C4_UI_Node* node) {
    if (!node) {
        return NULL;
    }
    if (node->input.isFocusable && node->input.isActive) {
        return node;
    }
    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node* found = C4_UI_Node_FindFocusable(child);
        if (found) {
            return found;
        }
        child = child->nextSibling;
    }
    return NULL;
}

bool C4_UI_Node_HandleAction(C4_UI_Node* node, C4_InputEvent event) {
    if (!node) {
        return false;
    }

    if (C4_UI_Interaction_HandleAction(&node->input, event)) {
        return true;
    }

    C4_UI_Node* child = node->firstChild;
    while (child) {
        if (C4_UI_Interaction_HandleAction(&node->input, event)) {
            return true;
        }
        child = child->nextSibling;
    }

    return false;
}

bool C4_UI_Node_HandleMouseEvents(C4_UI_Node* node, SDL_Event* event) {
    if (!node || !event) {
        return false;
    }
        
    C4_UI_Node* child = node->lastChild;
    while (child) {
        if (C4_UI_Node_HandleMouseEvents(child, event)) {
            return true;
        }
        child = child->prevSibling;
    }

    return C4_UI_Interaction_HandleMouseEvents(node->absoluteRect, &node->input, event);
}

void C4_UI_Node_Update(C4_UI_Node* node, float deltaTime) {
    if (!node) {
        return;
    }
    C4_UI_Interaction_Update(&node->input, deltaTime);
    
    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node_Update(child, deltaTime);
        child = child->nextSibling;
    }
}

void C4_UI_Node_AttachChild(C4_UI_Node* parent, C4_UI_Node* child) {
    child->parent = parent;
    if (!parent->firstChild) {
        parent->firstChild = child;
        parent->lastChild = child;
    } else {
        parent->lastChild->nextSibling = child;
        child->prevSibling = parent->lastChild;
        parent->lastChild = child;
    }
}

void C4_UI_Node_PushNode(C4_UI_Node* head, C4_UI_Node* newNode) {
    C4_UI_Node* current = head;
    while (current->nextSibling) {
        current = current->nextSibling;
    }
    current->nextSibling = newNode;
    newNode->prevSibling = current;
}

static void C4_UI_UpdateTextRect(C4_UI_Node* node) {
    if (!node || node->type != C4_UI_Type_Text || !node->text.textObject) {
        return;
    }

    int w, h;
    if (TTF_GetTextSize(node->text.textObject, &w, &h)) {
        node->rect.w = (float)w;
        node->rect.h = (float)h;
    }
}

void C4_UI_Node_SetTextString(C4_UI_Node* node, const char* newString) {
    if (!node || node->type != C4_UI_Type_Text || !node->text.textObject) return;
    
    TTF_SetTextString(node->text.textObject, newString, 0);
    C4_UI_UpdateTextRect(node);
}

void C4_UI_Node_ChangeFont(C4_UI_Node* node, TTF_Font* newFont) {
    if (!node || node->type != C4_UI_Type_Text || !node->text.textObject || !newFont) {
        return;
    }
    
    if (!TTF_SetTextFont(node->text.textObject, newFont)) {
        SDL_Log("Failed to set text font: %s", SDL_GetError());
    } else {
        node->text.font = newFont;
    }
    C4_UI_UpdateTextRect(node);
}

void C4_UI_Node_SetTextWrap(C4_UI_Node* node, int widthInPixels) {
    if (!node || node->type != C4_UI_Type_Text || !node->text.textObject) {
        return;
    }
    
    if (!TTF_SetTextWrapWidth(node->text.textObject, widthInPixels)) {
        SDL_Log("Failed to set wrap width: %s", SDL_GetError());
    }
    C4_UI_UpdateTextRect(node);
}

C4_UI_Node* C4_UI_Node_Create(C4_UI_Node_Config* config, float UIScale) {
    if (!config) {
        return NULL;
    }
    C4_UI_Node* node = calloc(1, sizeof(C4_UI_Node));
    if (!node) {
        SDL_Log("Unable to allocate memory for UI node");
        return NULL;
    }

    node->type = config->type;

    if (node->type == C4_UI_Type_Shape) {
        node->shape.type = config->shape.type;
        node->shape.rotationDegrees = 0.0f;
        node->shape.borderWidth = config->shape.borderWidth;
    } else if (node->type == C4_UI_Type_Text) {
        C4_UI_Data_Text_Config* text = &config->text;
        node->text.font = text->font;
        node->text.textObject = TTF_CreateText(text->textEngine, text->font, text->text, 0);
        SDL_Color color = config->style->inactive.text;
        TTF_SetTextColor(node->text.textObject, color.r, color.g, color.b, color.a);
        C4_UI_UpdateTextRect(node);
    }

    node->parent = NULL;
    node->nextSibling = NULL;
    node->prevSibling = NULL;

    node->input = (C4_UI_Interaction){
        .isHovered = false,
        .isPressed = false,
        .isFocusable = false,
        .isActive = false,
        .isRepeating = false,
        .OnHover = NULL,
        .WhilePressed = NULL,
        .OnPress = NULL,
        .OnRelease = NULL,
        .context = NULL,
        .timing = (C4_UI_Interaction_WhilePressedTiming){
            .delay = 0.5f,
            .interval = 0.1f,
            .pressTimer = 0.f
        }
    };

    node->style = *config->style;
    node->inheritState = false;
    node->rect = config->rect;
    C4_UI_Node_CalculateAbsoluteRect(node, UIScale, 0.f, 0.f);
    node->padding = 0;
    node->spacing = 0;
    node->direction = C4_UI_Direction_Vertical;
    node->fitContent = true;

    node->navUp = NULL;
    node->navDown = NULL;
    node->navLeft = NULL;
    node->navRight = NULL;

    return node;
}