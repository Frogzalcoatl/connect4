#include "Connect4/ui/elements/buttonStack.h"
#include <stdlib.h>

static SDL_FRect C4_UI_ButtonStack_GetUpdatedButtonRect(C4_UI_ButtonStack* stack, size_t index) {
    SDL_FRect rect = {0.f, 0.f, 0.f, 0.f};
    if (index >= stack->count) {
        SDL_Log("Unable to update button rect index %zu. Out of bounds.", index);
        return rect;
    }
    float totalMarginSpace = stack->margin * (stack->count - 1);
    if (stack->direction == C4_UI_ButtonStack_Direction_Vertical) {
        float buttonHeight = (stack->bounds.h - totalMarginSpace) / stack->count;
        rect.x = stack->bounds.x;
        rect.y = stack->bounds.y + (buttonHeight * index) + (stack->margin * index);
        rect.w = stack->bounds.w;
        rect.h = buttonHeight;
    } else {
        float buttonWidth = (stack->bounds.w - totalMarginSpace) / stack->count;
        rect.y = stack->bounds.y;
        rect.h = stack->bounds.h;
        rect.w = buttonWidth;
        rect.x = stack->bounds.x + (buttonWidth * index) + (stack->margin * index);
    }
    return rect;
}

bool C4_UI_ButtonStack_InitProperties(C4_UI_ButtonStack* stack, SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonStack_Direction direction, unsigned int margin, float buttonPtSize) {
    if (!stack) {
        SDL_Log("Unable to init button stack properties. Pointer is NULL");
        return false;
    }
    if (count == 0) {
        SDL_Log("Button stack count must be greater than 0");
        return false;
    }
    if (direction != C4_UI_ButtonStack_Direction_Vertical && direction != C4_UI_ButtonStack_Direction_Horizontal) {
        SDL_Log("Invalid button stack direction");
        return false;
    }
    stack->bounds = bounds;
    stack->direction = direction;
    stack->margin = margin;
    stack->count = count;
    stack->buttons = calloc(count, sizeof(C4_UI_Button));
    if (!stack->buttons) {
        SDL_Log("Unable to allocate memory for buttons in button stack.");
        return false;
    }
    for (size_t i = 0; i < stack->count; i++) {
        if (!C4_UI_Button_InitProperties(&stack->buttons[i], renderer, "", C4_FontType_Bold, buttonPtSize, C4_UI_ButtonStack_GetUpdatedButtonRect(stack, i))) {
            return false;
        }
    }
    return true;
}

C4_UI_ButtonStack* C4_UI_ButtonStack_Create(SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonStack_Direction direction, unsigned int margin, float buttonPtSize) {
    C4_UI_ButtonStack* stack = calloc(1, sizeof(C4_UI_ButtonStack));
    if (!stack) {
        SDL_Log("Unable to allocate memory for button stack");
        return NULL;
    }
    if (!C4_UI_ButtonStack_InitProperties(stack, renderer, bounds, count, direction, margin, buttonPtSize)) {
        C4_UI_ButtonStack_Destroy(stack);
        return NULL;
    }
    return stack;
}

void C4_UI_ButtonStack_FreeResources(C4_UI_ButtonStack* stack) {
    if (!stack) {
        return;
    }
    for (size_t i = 0; i < stack->count; i++) {
        C4_UI_Button_FreeResources(&stack->buttons[i]);
    }
    free(stack->buttons);
    stack->buttons = NULL;
}

void C4_UI_ButtonStack_Destroy(C4_UI_ButtonStack* stack) {
    if (!stack) {
        return;
    }
    C4_UI_ButtonStack_FreeResources(stack);
    free(stack);
}

void C4_UI_ButtonStack_SetButtonIndex(C4_UI_ButtonStack* stack, size_t buttonIndex, SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize) {
    if (buttonIndex >= stack->count) {
        SDL_Log("Unable to set button index %zu. Out of bounds.", buttonIndex);
        return;
    }
    stack->buttons[buttonIndex].text.ptSize = ptSize;
    stack->buttons[buttonIndex].text.font = C4_GetFont(font);
    C4_UI_Button_ChangeStr(&stack->buttons[buttonIndex], str, renderer);
}

void C4_UI_ButtonStack_Draw(C4_UI_ButtonStack* stack, SDL_Renderer* renderer) {
    if (!stack) {
        SDL_Log("Unable to draw button stack. Pointer is NULL");
        return;
    }
    for (size_t i = 0; i < stack->count; i++) {
        C4_UI_Button_Draw(&stack->buttons[i], renderer);
    }
}

long long C4_UI_ButtonStack_HandleMouseEvents(C4_UI_ButtonStack* stack, SDL_Event* event, SDL_Renderer* renderer) {
    if (!stack || !event || !renderer) {
        SDL_Log("Button stack, event, and/or renderer is NULL");
        return -1;
    }
    for (size_t i = 0; i < stack->count; i++) {
        if (C4_UI_Button_HandleMouseEvents(&stack->buttons[i], event, renderer)) {
            return i;
        }
    }
    return -1;
}

void C4_UI_ButtonStack_TransformResize(C4_UI_ButtonStack* stack, const SDL_FRect rect) {
    if (!stack) {
        return;
    }
    stack->bounds = rect;
    SDL_FRect buttonRect;
    for (size_t i = 0; i < stack->count; i++) {
        buttonRect = C4_UI_ButtonStack_GetUpdatedButtonRect(stack, i);
        C4_UI_Button_TransformResize(&stack->buttons[i], buttonRect.x, buttonRect.y, buttonRect.w, buttonRect.h);
    }
}

void C4_UI_ButtonStack_CenterInWindow(C4_UI_ButtonStack* stack, C4_Axis axis) {
    if (!stack) {
        return;
    }
    C4_UI_CenterInWindow(&stack->bounds, axis);
    C4_UI_ButtonStack_TransformResize(stack, stack->bounds);
}