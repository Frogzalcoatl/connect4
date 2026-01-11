#include "Connect4/ui/node.h"
#include "Connect4/ui/draw/shapes.h"
#include "Connect4/ui/draw/text.h"
#include "Connect4/ui/utils.h"
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
        node->rect = config->shape.rect;
    } else if (node->type == C4_UI_Type_Text) {
        C4_UI_Data_Text_Config* text = &config->text;
        node->text.font = text->font;
        node->text.textObject = TTF_CreateText(text->textEngine, text->font, text->text, 0);
        SDL_Color color = config->style->inactive.text;
        TTF_SetTextColor(node->text.textObject, color.r, color.g, color.b, color.a);
        C4_UI_UpdateTextRect(node);
        node->rect.x = config->text.posX;
        node->rect.y = config->text.posY;
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
    node->padding = 0;
    node->spacing = 0;
    node->direction = C4_UI_Direction_Vertical;
    node->childrenAlign = C4_UI_Align_TopLeft;

    node->navUp = NULL;
    node->navDown = NULL;
    node->navLeft = NULL;
    node->navRight = NULL;

    return node;
}

void C4_UI_Node_AlignChildren(C4_UI_Node* node, C4_UI_Axis axis) {
    if (!node) {
        return;
    }
    float minChildX = node->padding;
    float minChildY = node->padding;
    float maxChildX = node->rect.w - node->padding;
    float maxChildY = node->rect.h - node->padding;
    
    float parentCenterX = node->rect.w / 2.f;
    float parentCenterY = node->rect.h / 2.f;

    bool alignX = axis == C4_UI_Axis_X || axis == C4_UI_Axis_XY;
    bool alignY = axis == C4_UI_Axis_Y || axis == C4_UI_Axis_XY;

    C4_UI_Node* current = node->firstChild;
    while (current) {
        SDL_FPoint newPos;
        switch (node->childrenAlign) {
            case C4_UI_Align_TopLeft: {
                newPos.x = minChildX;
                newPos.y = minChildY;
            }; break;
            case C4_UI_Align_Top: {
                newPos.x = parentCenterX - current->rect.w / 2.f;
                newPos.y = minChildY;
            }; break;
            case C4_UI_Align_TopRight: {
                newPos.x = maxChildX - current->rect.w;
                newPos.y = minChildY;
            }; break;
            case C4_UI_Align_CenterLeft: {
                newPos.x = minChildX;
                newPos.y = parentCenterY - current->rect.h / 2.f;
            }; break;
            case C4_UI_Align_Center: {
                newPos.x = parentCenterX - current->rect.w / 2.f;
                newPos.y = parentCenterY - current->rect.h / 2.f;
            }; break;
            case C4_UI_Align_CenterRight: {
                newPos.x = maxChildX - current->rect.w;
                newPos.y = parentCenterY - current->rect.h / 2.f;
            }; break;
            case C4_UI_Align_BottomLeft: {
                newPos.x = minChildX;
                newPos.y = maxChildY - current->rect.h;
            }; break;
            case C4_UI_Align_Bottom: {
                newPos.x = parentCenterX - current->rect.w / 2.f;
                newPos.y = maxChildY - current->rect.h;
            } case C4_UI_Align_BottomRight: {
                newPos.x = maxChildX - current->rect.w;
                newPos.y = maxChildY - current->rect.h;
            }
        }
        if (alignX) {
            current->rect.x = newPos.x;
        }
        if (alignY) {
            current->rect.y = newPos.y;
        }
        current = current->nextSibling;
    }
}

void C4_UI_Node_ApplyChildSpacing(C4_UI_Node* parent) {
    if (!parent) {
        return;
    }

    parent->rect.w = parent->padding;
    parent->rect.h = parent->padding;
    if (parent->direction == C4_UI_Direction_Horizontal) {
        parent->rect.w *= 2.f;
    } else {
        parent->rect.h *= 2.f;
    }
    C4_UI_Node* current = parent->firstChild;
    while (current) {
        if (parent->direction == C4_UI_Direction_Horizontal) {
            current->rect.x = parent->rect.w - parent->padding;
            current->rect.y = parent->padding;
            parent->rect.w += current->rect.w + parent->spacing;
            parent->rect.h = C4_FMax(parent->rect.h, current->rect.h + parent->padding * 2.f);
        } else {
            current->rect.x = parent->padding;
            current->rect.y = parent->rect.h - parent->padding;
            parent->rect.w = C4_FMax(parent->rect.w, current->rect.w + parent->padding * 2.f);
            parent->rect.h += current->rect.h + parent->spacing;
        }
        current = current->nextSibling;
    }
    if (parent->direction == C4_UI_Direction_Horizontal) {
        parent->rect.w -= parent->spacing;
    } else {
        parent->rect.h -= parent->spacing;
    }
}

void C4_UI_Node_ClampToWindow(C4_UI_Node* node, unsigned int windowWidth, unsigned int windowHeight, float UIScale) {
    if (!node) {
        return;
    }

    SDL_FRect* rect = &node->rect;

    float visualWidth = rect->w * UIScale;
    float visualHeight = rect->h * UIScale;

    float scaledRightEdge = (rect->x * UIScale) + visualWidth;
    if (scaledRightEdge > (float)windowWidth) {
        rect->x = ((float)windowWidth / UIScale) - rect->w;
    }
    if (rect->x < 0.f) {
        rect->x = 0.f;
    }

    float scaledBottomEdge = (rect->y * UIScale) + visualHeight;
    if (scaledBottomEdge > (float)windowHeight) {
        rect->y = ((float)windowHeight / UIScale) - rect->h;
    }
    if (rect->y < 0.f) {
        rect->y = 0.f;
    }
}

void C4_UI_CenterInWindow(C4_UI_Node* node, C4_UI_Axis axis, unsigned int windowWidth, unsigned int windowHeight, float UIScale) {
    if (!node) {
        return;
    }
    SDL_FRect* rect = &node->rect;

    if (axis == C4_UI_Axis_X || axis == C4_UI_Axis_XY) {
        rect->x = (windowWidth / 2.f) - (rect->w * UIScale / 2.f);
    }

    if (axis == C4_UI_Axis_Y || axis == C4_UI_Axis_XY) {
        rect->y = (windowHeight / 2.f) - (rect->h * UIScale / 2.f);
    }

    C4_UI_Node_ClampToWindow(node, windowWidth, windowHeight, UIScale);
}