#include "Connect4/ui/elements/buttonGroup.h"
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

bool C4_UI_ButtonGroup_InitProperties(C4_UI_ButtonGroup* group, SDL_Renderer* renderer, const C4_UI_ButtonGroup_Config* config) {
    if (!group) {
        SDL_Log("Unable to init button group properties. Pointer is NULL");
        return false;
    }
    if (!config) {
        return false;
    }
    if (!config->theme) {
        SDL_Log("Unable to init button group properties. Theme is NULL");
        return false;
    }
    if (config->count == 0) {
        SDL_Log("Button group count must be greater than 0");
        return false;
    }
    if (
        config->buttonDirection != C4_UI_ButtonGroup_Direction_Vertical &&
        config->buttonDirection != C4_UI_ButtonGroup_Direction_Horizontal
    ) {
        SDL_Log("Invalid button group direction");
        return false;
    }
    group->bounds = config->destination;
    group->direction = config->buttonDirection;
    group->margin = config->margin;
    group->count = config->count;
    group->buttons = calloc(group->count, sizeof(C4_UI_Button));
    if (!group->buttons) {
        SDL_Log("Unable to allocate memory for buttons in button group.");
        return false;
    }
    for (size_t i = 0; i < group->count; i++) {
        if (
            !C4_UI_Button_InitProperties(
                &group->buttons[i], renderer,
                &(C4_UI_Button_Config){
                    .text = "",
                    .destination = C4_UI_ButtonGroup_GetUpdatedButtonRect(group, i),
                    .symbol = C4_UI_SymbolType_None,
                    .font = config->font,
                    .theme = config->theme
                }
            )
        ) {
            return false;
        }
    }
    return true;
}

C4_UI_ButtonGroup* C4_UI_ButtonGroup_Create(SDL_Renderer* renderer, const C4_UI_ButtonGroup_Config* config) {
    C4_UI_ButtonGroup* group = calloc(1, sizeof(C4_UI_ButtonGroup));
    if (!group) {
        SDL_Log("Unable to allocate memory for button group");
        return NULL;
    }
    if (!C4_UI_ButtonGroup_InitProperties(group, renderer, config)) {
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

void C4_UI_ButtonGroup_Destroy(void* data) {
    if (!data) {
        return;
    }
    C4_UI_ButtonGroup* group = (C4_UI_ButtonGroup*)data;
    C4_UI_ButtonGroup_FreeResources(group);
    free(group);
}

void C4_UI_ButtonGroup_Draw(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY) {
    if (!data) {
        return;
    }
    C4_UI_ButtonGroup* group = (C4_UI_ButtonGroup*)data;
    for (size_t i = 0; i < group->count; i++) {
        C4_UI_Button_Draw(&group->buttons[i], renderer, scale, parentX, parentY);
    }
}

void C4_UI_ButtonGroup_Update(void* data, float deltaTime) {
    if (!data) {
        return;
    }
    C4_UI_ButtonGroup* group = (C4_UI_ButtonGroup*)data;
    for (size_t i = 0; i < group->count; i++) {
        C4_UI_Button_Update(&group->buttons[i], deltaTime);
    }
}

void C4_UI_ButtonGroup_HandleMouseEvents(void* data, SDL_Event* event, float scale, float parentX, float parentY) {
    if (!data || !event) {
        SDL_Log("Button group, event, and/or renderer is NULL");
        return;
    }
    C4_UI_ButtonGroup* group = (C4_UI_ButtonGroup*)data;
    for (size_t i = 0; i < group->count; i++) {
        C4_UI_Button_HandleMouseEvents(&group->buttons[i], event, scale, parentX, parentY);
    }
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

void C4_UI_ButtonGroup_CenterInWindow(C4_UI_ButtonGroup* group, C4_Axis axis, unsigned int windowWidth, unsigned int windowHeight) {
    if (!group) {
        return;
    }
    C4_UI_CenterInWindow(&group->bounds, axis, windowWidth, windowHeight);
    C4_UI_ButtonGroup_TransformResize(group, group->bounds);
}

// void* to work in container
void C4_UI_ButtonGroup_Reset(void* data) {
    if (!data) {
        return;
    }
    C4_UI_ButtonGroup* group = (C4_UI_ButtonGroup*)data;
    for (size_t i = 0; i < group->count; i++) {
        C4_UI_Button_Reset(&group->buttons[i]);
    }
}