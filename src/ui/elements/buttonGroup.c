#include "Connect4/ui/elements/buttonGroup.h"
#include "Connect4/constants.h"
#include <stdlib.h>

static SDL_FRect C4_UI_ButtonGroup_GetUpdatedButtonRect(C4_UI_ButtonGroup* group, size_t index) {
    SDL_FRect rect = {0.f, 0.f, 0.f, 0.f};
    if (index >= group->count) {
        SDL_Log("Unable to update button rect index %zu. Out of bounds.", index);
        return rect;
    }
    float totalMarginSpace = (float)group->margin * (group->count > 1 ? ((float)group->count - 1.f) : 0.f);
    if (group->direction == C4_UI_ButtonGroup_Direction_Vertical) {
        float buttonHeight = (group->bounds.h - totalMarginSpace) / (float)group->count;
        rect.x = group->bounds.x;
        rect.y = group->bounds.y + (buttonHeight * (float)index) + ((float)group->margin * (float)index);
        rect.w = group->bounds.w;
        rect.h = buttonHeight;
    } else {
        float buttonWidth = (group->bounds.w - totalMarginSpace) / (float)group->count;
        rect.y = group->bounds.y;
        rect.h = group->bounds.h;
        rect.w = buttonWidth;
        rect.x = group->bounds.x + (buttonWidth * (float)index) + ((float)group->margin * (float)index);
    }
    return rect;
}

bool C4_UI_ButtonGroup_InitProperties(C4_UI_ButtonGroup* group, SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonGroup_Direction direction, unsigned int margin, const C4_UI_Theme* theme) {
    if (!group) {
        SDL_Log("Unable to init button group properties. Pointer is NULL");
        return false;
    }
    if (!theme) {
        SDL_Log("Unable to init button group properties. Theme is NULL");
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
        if (!C4_UI_Button_InitProperties(&group->buttons[i], renderer, "", C4_UI_ButtonGroup_GetUpdatedButtonRect(group, i), C4_UI_SymbolType_None, 1.f, 1.f, 0, theme)) {
            return false;
        }
    }
    return true;
}

C4_UI_ButtonGroup* C4_UI_ButtonGroup_Create(SDL_Renderer* renderer, const SDL_FRect bounds, size_t count, C4_UI_ButtonGroup_Direction direction, unsigned int margin, const C4_UI_Theme* theme) {
    C4_UI_ButtonGroup* group = calloc(1, sizeof(C4_UI_ButtonGroup));
    if (!group) {
        SDL_Log("Unable to allocate memory for button group");
        return NULL;
    }
    if (!C4_UI_ButtonGroup_InitProperties(group, renderer, bounds, count, direction, margin, theme)) {
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

void C4_UI_ButtonGroup_SetButtonIndex(
    C4_UI_ButtonGroup* group, size_t buttonIndex, SDL_Renderer* renderer, const char* str,C4_UI_SymbolType symbol,
    float symbolWidth, float symbolHeight, int symbolRotationDegrees, const C4_UI_Theme* theme
) {
    if (!group) {
        SDL_Log("Unable to set button index %zu. Button group is NULL", buttonIndex);
        return;
    }
    if (buttonIndex >= group->count) {
        SDL_Log("Unable to set button index %zu. Out of bounds.", buttonIndex);
        return;
    }
    C4_UI_Button* btn = &group->buttons[buttonIndex];
    C4_UI_Button_InitProperties(
        btn, renderer, str, btn->background.destination, symbol,
        symbolWidth, symbolHeight, symbolRotationDegrees, theme
    );
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

int C4_UI_ButtonGroup_HandleMouseEvents(C4_UI_ButtonGroup* group, SDL_Event* event) {
    if (!group || !event) {
        SDL_Log("Button group, event, and/or renderer is NULL");
        return -1;
    }
    for (size_t i = 0; i < group->count; i++) {
        if (C4_UI_Button_HandleMouseEvents(&group->buttons[i], event)) {
            return (int)i;
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