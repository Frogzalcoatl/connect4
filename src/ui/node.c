#include "Connect4/ui/node.h"
#include "Connect4/ui/draw/shapes.h"
#include "Connect4/ui/draw/text.h"
#include "Connect4/ui/utils.h"
#include "Connect4/ui/memoryArena.h"
#include "Connect4/game/consoleOutput.h"
#include <assert.h>
#include <stdlib.h>

void C4_UI_Node_CleanupResources(C4_UI_Node* node) {
    if (!node) {
        return;
    }

    if (
        node->type == C4_UI_Type_Text
    ) {
        if (node->text.textObject) {
            TTF_DestroyText(node->text.textObject);
            node->text.textObject = NULL;
        }
        if (node->text.storage) {
            SDL_free(node->text.storage);
            node->text.storage = NULL;
        }
    }

    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node_CleanupResources(child);
        child = child->nextSibling;
    }
}

void C4_UI_Node_CalculateLayout(C4_UI_Node* node, float scale, float parentX, float parentY) {
    assert(node);

    node->absoluteRect.x = parentX + (node->rect.x * scale);
    node->absoluteRect.y = parentY + (node->rect.y * scale);
    node->absoluteRect.w = node->rect.w * scale;
    node->absoluteRect.h = node->rect.h * scale;

    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node_CalculateLayout(child, scale, node->absoluteRect.x, node->absoluteRect.y);
        child = child->nextSibling;
    }
}

void C4_UI_Node_Draw(C4_UI_Node* node, SDL_Renderer* renderer, float uiScale) {
    assert(node);

    C4_UI_Interaction* activeInput = &node->input;
    
    if (node->inheritState && node->parent) {
        activeInput = &node->parent->input;
    }

    C4_UI_StyleState* currentStyle = &node->style.normal;
    if (node->input.isFocusable || node->inheritState) {
        if (activeInput->isPressed) {
            currentStyle = &node->style.pressed;
        } else if (activeInput->isHovered) {
            currentStyle = &node->style.hovered;
        } else if (!activeInput->isActive) {
            currentStyle = &node->style.inactive;
        }
    }

    switch (node->type) {
        case C4_UI_Type_Shape: {
            C4_UI_DrawShape(node->absoluteRect, &node->shape, currentStyle, node->mirror, renderer, uiScale);
        }; break;
        case C4_UI_Type_Text: {
            C4_UI_DrawText(node->absoluteRect, &node->text, currentStyle, node->mirror, renderer, uiScale);
        }; break;
        default: break;
    }

    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node_Draw(child, renderer, uiScale);
        child = child->nextSibling;
    }
}

C4_UI_Node* C4_UI_Node_FindFocusable(C4_UI_Node* node) {
    assert(node);

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
    assert(node);

    if (C4_UI_Interaction_HandleAction(&node->input, event)) {
        return true;
    }

    C4_UI_Node* child = node->firstChild;
    while (child) {
        if (C4_UI_Node_HandleAction(child, event)) { 
            return true;
        }
        child = child->nextSibling;
    }

    return false;
}

bool C4_UI_Node_HandleMouseEvents(C4_UI_Node* node, SDL_Event* event) {
    assert(node && event);
        
    C4_UI_Node* child = node->lastChild;
    while (child) {
        if (C4_UI_Node_HandleMouseEvents(child, event)) {
            return true;
        }
        child = child->prevSibling;
    }

    C4_UI_ShapeType shapeType = node->type == C4_UI_Type_Shape ? node->shape.type : C4_UI_ShapeType_Rectangle;

    float rotationDegrees = node->type == C4_UI_Type_Shape ? node->shape.rotationDegrees : 0.f;

    return C4_UI_Interaction_HandleMouseEvents(&node->input, event, shapeType, node->absoluteRect, rotationDegrees, node->mirror);
}

void C4_UI_Node_Update(C4_UI_Node* node, float deltaTime) {
    assert(node);

    C4_UI_Interaction_Update(&node->input, deltaTime);
    
    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node_Update(child, deltaTime);
        child = child->nextSibling;
    }
}

void C4_UI_Node_AttachChild(C4_UI_Node* parent, C4_UI_Node* child) {
    assert(parent && child);

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
    assert(head && newNode);

    C4_UI_Node* current = head;
    while (current->nextSibling) {
        current = current->nextSibling;
    }
    current->nextSibling = newNode;
    newNode->prevSibling = current;
}

static void C4_UI_Node_UpdateTextRect(C4_UI_Node* node) {
    assert(node && node->text.textObject && node->type == C4_UI_Type_Text);

    int w, h;
    if (TTF_GetTextSize(node->text.textObject, &w, &h)) {
        node->rect.w = (float)w;
        node->rect.h = (float)h;
    }
}

void C4_UI_Node_SetTextString(C4_UI_Node* node, const char* newString) {
    assert(node && node->text.textObject && node->type == C4_UI_Type_Text);

    if (node->text.storage && strcmp(node->text.storage, newString) == 0) {
        return;
    }

    if (node->text.storage) {
        SDL_free(node->text.storage);
    }

    node->text.storage = SDL_strdup(newString);

    TTF_SetTextString(node->text.textObject, node->text.storage, 0);
    
    if (node->parent) {
        C4_UI_Node_AlignChildren(node->parent, C4_UI_Axis_XY);
    }
    C4_UI_Node_UpdateTextRect(node);
}

void C4_UI_Node_ChangeFont(C4_UI_Node* node, TTF_Font* newFont) {
    assert(node && node->text.textObject && node->type == C4_UI_Type_Text);
    
    if (!TTF_SetTextFont(node->text.textObject, newFont)) {
        C4_Warn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to set text font: %s",
            SDL_GetError()
        );
    } else {
        node->text.font = newFont;
    }
    C4_UI_Node_UpdateTextRect(node);
}

void C4_UI_Node_SetTextWrap(C4_UI_Node* node, int widthInPixels) {
    assert(node && node->text.textObject && node->type == C4_UI_Type_Text);
    
    if (!TTF_SetTextWrapWidth(node->text.textObject, widthInPixels)) {
        C4_Warn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Failed to set text wrap width: %s",
            SDL_GetError()
        );
    }
    C4_UI_Node_UpdateTextRect(node);
}

C4_UI_Node* C4_UI_Node_Create(C4_MemoryArena* arena, C4_UI_Node_Config* config) {
    assert(arena && config);

    C4_UI_Node* node = C4_Arena_Alloc(arena, sizeof(C4_UI_Node));
    if (!node) {
        C4_FatalError(
            C4_ErrorCode_OutOfMemory, 
            "Unable to allocate memory for ui node"
        );
    }

    node->type = config->type;

    if (node->type == C4_UI_Type_Shape) {
        assert(config->shape);
        
        node->shape.type = config->shape->type;
        node->shape.rotationDegrees = 0.0f;
        node->shape.borderWidth = config->shape->borderWidth;
        node->rect = config->shape->rect;
        
    } else if (node->type == C4_UI_Type_Text) {
        assert(config->text);

        C4_UI_Data_Text_Config* text = config->text;
        node->text.font = text->font;

        size_t strLen = strlen(config->text->text) + 1;
        node->text.storage = SDL_calloc(1, strLen);
        strcpy(node->text.storage, config->text->text);

        node->text.textObject = TTF_CreateText(text->textEngine, text->font, node->text.storage, 0);

        SDL_Color color = config->style->inactive.text;
        TTF_SetTextColor(node->text.textObject, color.r, color.g, color.b, color.a);

        if (config->text->uiScale <= 0.1f) {
            config->text->uiScale = 0.1f;
        }

        C4_UI_Node_UpdateTextRect(node);
        node->rect.x = config->text->posX;
        node->rect.y = config->text->posY;
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
        .OnCancel = NULL,
        .context = NULL,
        .sounds = (C4_UI_Interaction_Sounds){
          .onHover = C4_SoundEffect_None,
          .whilePressed = C4_SoundEffect_None,
          .onPress = C4_SoundEffect_None,
          .onRelease = C4_SoundEffect_None,
          .onCancel = C4_SoundEffect_None
        },
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
    node->direction = C4_UI_Direction_None;
    node->childrenAlign = C4_UI_Align_TopLeft;
    node->selfAlign = C4_UI_Align_None;
    node->mirror = C4_UI_Mirror_None;

    node->navUp = NULL;
    node->navDown = NULL;
    node->navLeft = NULL;
    node->navRight = NULL;

    return node;
}

static void C4_UI_AlignHelper(
    SDL_FPoint* newPos,
    SDL_FRect rect,
    C4_UI_Align alignType,
    float minChildX,
    float minChildY,
    float maxChildX,
    float maxChildY,
    float parentCenterX,
    float parentCenterY
) {
    assert(newPos);
    assert(alignType >= C4_UI_Align_TopLeft && alignType < C4_UI_Align_Count);

    switch (alignType) {
        case C4_UI_Align_TopLeft: {
            newPos->x = minChildX;
            newPos->y = minChildY;
        }; break;
        case C4_UI_Align_Top: {
            newPos->x = parentCenterX - rect.w / 2.f;
            newPos->y = minChildY;
        }; break;
        case C4_UI_Align_TopRight: {
            newPos->x = maxChildX - rect.w;
            newPos->y = minChildY;
        }; break;
        case C4_UI_Align_CenterLeft: {
            newPos->x = minChildX;
            newPos->y = parentCenterY - rect.h / 2.f;
        }; break;
        case C4_UI_Align_Center: {
            newPos->x = parentCenterX - rect.w / 2.f;
            newPos->y = parentCenterY - rect.h / 2.f;
        }; break;
        case C4_UI_Align_CenterRight: {
            newPos->x = maxChildX - rect.w;
            newPos->y = parentCenterY - rect.h / 2.f;
        }; break;
        case C4_UI_Align_BottomLeft: {
            newPos->x = minChildX;
            newPos->y = maxChildY - rect.h;
        }; break;
        case C4_UI_Align_Bottom: {
            newPos->x = parentCenterX - rect.w / 2.f;
            newPos->y = maxChildY - rect.h;
        }; break;
        case C4_UI_Align_BottomRight: {
            newPos->x = maxChildX - rect.w;
            newPos->y = maxChildY - rect.h;
        }; break;
        default: break;
    }
}

void C4_UI_Node_AlignChildren(C4_UI_Node* node, C4_UI_Axis axis) {
    assert(node);

    if (node->childrenAlign == C4_UI_Align_None) {
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
        C4_UI_AlignHelper(
            &newPos,
            current->rect,
            node->childrenAlign,
            minChildX,
            minChildY,
            maxChildX,
            maxChildY,
            parentCenterX,
            parentCenterY
        );
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
    assert(parent);

    if (!parent->firstChild) {
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
            parent->rect.h = SDL_max(parent->rect.h, current->rect.h + parent->padding * 2.f);
        } else {
            current->rect.x = parent->padding;
            current->rect.y = parent->rect.h - parent->padding;
            parent->rect.w = SDL_max(parent->rect.w, current->rect.w + parent->padding * 2.f);
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

void C4_UI_Node_ClampToWindow(C4_UI_Node* node, unsigned int windowWidth, unsigned int windowHeight) {
    assert(node);

    SDL_FRect* rect = &node->rect;

    float rightEdge = rect->x + rect->w;
    if (rightEdge > (float)windowWidth) {
        rect->x = (float)windowWidth - rect->w;
    }
    if (rect->x < 0.f) {
        rect->x = 0.f;
    }

    float bottomEdge = rect->y + rect->h;
    if (bottomEdge > (float)windowHeight) {
        rect->y = (float)windowHeight - rect->h;
    }
    if (rect->y < 0.f) {
        rect->y = 0.f;
    }
}

void C4_UI_Node_AlignInParent(C4_UI_Node* node, SDL_FRect parentRect) {
    assert(node);
    
    if (node->selfAlign == C4_UI_Align_None) {
        return;
    }

    SDL_FPoint newPos;
    C4_UI_AlignHelper(
        &newPos,
        node->rect,
        node->selfAlign,
        parentRect.x,
        parentRect.y,
        (float)parentRect.w,
        (float)parentRect.h,
        parentRect.x + (parentRect.w / 2.f),
        parentRect.y + (parentRect.h / 2.f)
    );
    node->rect.x = newPos.x;
    node->rect.y = newPos.y;
}

void C4_UI_Node_Reset(C4_UI_Node* node) {
    assert(node);

    C4_UI_Interaction_Reset(&node->input);

    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node_Reset(child);
        child = child->nextSibling;
    }
}

void C4_UI_Node_RefreshLayout(C4_UI_Node* node, SDL_FRect parentRect) {
    if (!node) {
        return;
    }

    SDL_FRect internalRect = {0.f, 0.f, node->rect.w, node->rect.h};

    C4_UI_Node* child = node->firstChild;
    while (child) {
        C4_UI_Node_RefreshLayout(child, internalRect);
        child = child->nextSibling;
    }

    if (node->direction != C4_UI_Direction_None) {
        C4_UI_Node_ApplyChildSpacing(node);
    }

    C4_UI_Axis childrenAlignAxis;
    if (node->direction == C4_UI_Direction_Vertical) {
        childrenAlignAxis = C4_UI_Axis_X;
    } else if (node->direction == C4_UI_Direction_Horizontal) {
        childrenAlignAxis = C4_UI_Axis_Y;
    } else {
        childrenAlignAxis = C4_UI_Axis_XY;
    }
    C4_UI_Node_AlignChildren(node, childrenAlignAxis);

    if (node->selfAlign != C4_UI_Align_None) {
        C4_UI_Node_AlignInParent(node, parentRect);
    }
}