#include "Connect4/ui/rectangle_element.h"
#include "Connect4/constants.h"
#include <stdlib.h>

C4_UI_Rectangle* C4_UI_Rectangle_Create(const SDL_FRect rectangle, const SDL_Color color) {
    C4_UI_Rectangle* element = calloc(1, sizeof(C4_UI_Rectangle));
    if (!element) {
        SDL_Log("Unable to allocate memory for rectangle element");
        return NULL;
    }
    element->rectangle = rectangle;
    element->color = color;
    return element;
}

void C4_UI_Rectangle_Destroy(C4_UI_Rectangle* element) {
    if (!element) {
        SDL_Log("Tried to destroy NULL rectangle element");
        return;
    }
    free(element);
}

void C4_UI_Rectangle_Draw(C4_UI_Rectangle* element, SDL_Renderer* renderer) {
    if (!element) {
        SDL_Log("Rectangle element is NULL");
        return;
    }
    SDL_SetRenderDrawColor(renderer, element->color.r, element->color.g, element->color.b, element->color.a);
    SDL_RenderFillRect(renderer, &element->rectangle);
}

void C4_UI_Rectangle_CenterInWindow(C4_UI_Rectangle* element, C4_Axis axis) {
    if (!element) {
        SDL_Log("Rectangle element is NULL");
        return;
    }
    if (axis == C4_Axis_X || axis == C4_Axis_XY) {
        element->rectangle.x = (C4_BASE_WINDOW_WIDTH / 2.f) - (element->rectangle.w / 2.f);
    }
    if (axis == C4_Axis_Y || axis == C4_Axis_XY) {
        element->rectangle.y = (C4_BASE_WINDOW_HEIGHT / 2.f) - (element->rectangle.h / 2.f);
    }
}