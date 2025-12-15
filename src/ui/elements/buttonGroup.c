#include "Connect4/ui/elements/buttonGroup.h"
#include <stdlib.h>

static SDL_FRect C4_UI_ButtonGroup_GetUpdatedButtonRect(C4_UI_ButtonGroup* group, size_t index) {
    SDL_FRect rect = {0.f, 0.f, 0.f, 0.f};
    if (index >= group->count) {
        SDL_Log("Unable to update button rect index %zu. Out of bounds.", index);
        return rect;
    }
    float totalMarginSpace = group->margin * (group->count - 1);
    if (group->direction == C4_UI_ButtonGroup_Direction_Vertical) {
        float buttonHeight = (group->bounds.h - totalMarginSpace) / group->count;
        rect.x = group->bounds.x;
        rect.y = group->bounds.y + (buttonHeight * index) + (group->margin * index);
        rect.w = group->bounds.w;
        rect.h = buttonHeight;
    } else {
        float buttonWidth = (group->bounds.w - totalMarginSpace) / group->count;
        rect.y = group->bounds.y;
        rect.h = group->bounds.h;
        rect.w = buttonWidth;
        rect.x = group->bounds.x + (buttonWidth * index) + (group->margin * index);
    }
    return rect;
}

bool C4_UI_ButtonGroup_InitProperties(C4_UI_ButtonGroup* group, SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonGroup_Direction direction, unsigned int margin, float buttonPtSize) {
    if (!group) {
        SDL_Log("Unable to init button group properties. Pointer is NULL");
        return false;
    }
    if (count == 0) {
        SDL_Log("Button group count must be greater than 0");
        return false;
    }
    if (direction != C4_UI_ButtonGroup_Direction_Vertical && direction != C4_UI_ButtonGroup_Direction_Horizontal) {
        SDL_Log("Invalid button group direction");
        return false;
    }
    group->bounds = bounds;
    group->direction = direction;
    group->margin = margin;
    group->count = count;
    group->buttons = calloc(count, sizeof(C4_UI_Button));
    if (!group->buttons) {
        SDL_Log("Unable to allocate memory for buttons in button group.");
        return false;
    }
    for (size_t i = 0; i < group->count; i++) {
        if (!C4_UI_Button_InitProperties(&group->buttons[i], renderer, "", C4_FontType_Bold, buttonPtSize, C4_UI_ButtonGroup_GetUpdatedButtonRect(group, i))) {
            return false;
        }
    }
    return true;
}

C4_UI_ButtonGroup* C4_UI_ButtonGroup_Create(SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonGroup_Direction direction, unsigned int margin, float buttonPtSize) {
    C4_UI_ButtonGroup* group = calloc(1, sizeof(C4_UI_ButtonGroup));
    if (!group) {
        SDL_Log("Unable to allocate memory for button group");
        return NULL;
    }
    if (!C4_UI_ButtonGroup_InitProperties(group, renderer, bounds, count, direction, margin, buttonPtSize)) {
        C4_UI_ButtonGroup_Destroy(group);
        return NULL;
    }
    return group;
}

void C4_UI_ButtonGroup_FreeResources(C4_UI_ButtonGroup* group) {
    if (!group) {
        return;
    }
    for (size_t i = 0; i < group->count; i++) {
        C4_UI_Button_FreeResources(&group->buttons[i]);
    }
    free(group->buttons);
    group->buttons = NULL;
}

void C4_UI_ButtonGroup_Destroy(C4_UI_ButtonGroup* group) {
    if (!group) {
        return;
    }
    C4_UI_ButtonGroup_FreeResources(group);
    free(group);
}

void C4_UI_ButtonGroup_SetButtonIndex(C4_UI_ButtonGroup* group, size_t buttonIndex, SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize) {
    if (buttonIndex >= group->count) {
        SDL_Log("Unable to set button index %zu. Out of bounds.", buttonIndex);
        return;
    }
    group->buttons[buttonIndex].text.ptSize = ptSize;
    group->buttons[buttonIndex].text.font = C4_GetFont(font);
    C4_UI_Button_ChangeStr(&group->buttons[buttonIndex], str, renderer);
}

void C4_UI_ButtonGroup_Draw(C4_UI_ButtonGroup* group, SDL_Renderer* renderer) {
    if (!group) {
        SDL_Log("Unable to draw button group. Pointer is NULL");
        return;
    }
    for (size_t i = 0; i < group->count; i++) {
        C4_UI_Button_Draw(&group->buttons[i], renderer);
    }
}

long long C4_UI_ButtonGroup_HandleMouseEvents(C4_UI_ButtonGroup* group, SDL_Event* event, SDL_Renderer* renderer) {
    if (!group || !event || !renderer) {
        SDL_Log("Button group, event, and/or renderer is NULL");
        return -1;
    }
    for (size_t i = 0; i < group->count; i++) {
        if (C4_UI_Button_HandleMouseEvents(&group->buttons[i], event, renderer)) {
            return i;
        }
    }
    return -1;
}

void C4_UI_ButtonGroup_TransformResize(C4_UI_ButtonGroup* group, const SDL_FRect rect) {
    if (!group) {
        return;
    }
    group->bounds = rect;
    SDL_FRect buttonRect;
    for (size_t i = 0; i < group->count; i++) {
        buttonRect = C4_UI_ButtonGroup_GetUpdatedButtonRect(group, i);
        C4_UI_Button_TransformResize(&group->buttons[i], buttonRect.x, buttonRect.y, buttonRect.w, buttonRect.h);
    }
}

void C4_UI_ButtonGroup_CenterInWindow(C4_UI_ButtonGroup* group, C4_Axis axis) {
    if (!group) {
        return;
    }
    C4_UI_CenterInWindow(&group->bounds, axis);
    C4_UI_ButtonGroup_TransformResize(group, group->bounds);
}