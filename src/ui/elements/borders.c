#include "Connect4/ui/elements/borders.h"
#include "Connect4/constants.h"
#include <stdlib.h>

bool C4_UI_Borders_InitProperties(C4_UI_Borders* element, const SDL_FRect rectangle, const SDL_Color color, const unsigned int borderWidth) {
    if (!element) {
        SDL_Log("Unable to init properties for border element. Pointer is NULL");
        return false;
    }
    element->rectangle = rectangle;
    element->color = color;
    element->borderWidth = borderWidth;
    return true;
}

C4_UI_Borders* C4_UI_Borders_Create(const SDL_FRect rectangle, const SDL_Color color, const unsigned int borderWidth) {
    C4_UI_Borders* element = calloc(1, sizeof(C4_UI_Borders));
    if (!element) {
        SDL_Log("Unable to allocate memory for borders element");
        return NULL;
    }
    if (!C4_UI_Borders_InitProperties(element, rectangle, color, borderWidth)) {
        C4_UI_Borders_Destroy(element);
        return NULL;
    }
    return element;
}

void C4_UI_Borders_Destroy(C4_UI_Borders* element) {
    if (!element) {
        SDL_Log("Tried to destroy NULL borders element");
        return;
    }
    free(element);
}

void C4_UI_Borders_Draw(C4_UI_Borders* element, SDL_Renderer* renderer) {
    if (!element) {
        SDL_Log("Borders element is NULL");
        return;
    }
    SDL_SetRenderDrawColor(renderer, element->color.r, element->color.g, element->color.b, element->color.a);
    SDL_FRect topBorder = {element->rectangle.x, element->rectangle.y, element->rectangle.w, element->borderWidth};
    SDL_FRect bottomBorder = {element->rectangle.x, element->rectangle.y + element->rectangle.h - element->borderWidth, element->rectangle.w, element->borderWidth};
    SDL_FRect leftBorder = {element->rectangle.x, element->rectangle.y, element->borderWidth, element->rectangle.h};
    SDL_FRect rightBorder = {element->rectangle.x + element->rectangle.w - element->borderWidth, element->rectangle.y, element->borderWidth, element->rectangle.h};
    SDL_RenderFillRect(renderer, &topBorder);
    SDL_RenderFillRect(renderer, &bottomBorder);
    SDL_RenderFillRect(renderer, &leftBorder);
    SDL_RenderFillRect(renderer, &rightBorder);
}

void C4_UI_Borders_CenterInWindow(C4_UI_Borders* element, C4_Axis axis) {
    if (!element) {
        SDL_Log("Borders element is NULL");
        return;
    }
    if (axis == C4_Axis_X || axis == C4_Axis_XY) {
        element->rectangle.x = (C4_BASE_WINDOW_WIDTH / 2.f) - (element->rectangle.w / 2.f);
    }
    if (axis == C4_Axis_Y || axis == C4_Axis_XY) {
        element->rectangle.y = (C4_BASE_WINDOW_HEIGHT / 2.f) - (element->rectangle.h / 2.f);
    }
}