#include "Connect4/ui/elements/rectangle.h"
#include "Connect4/constants.h"
#include <stdlib.h>

bool C4_UI_Rectangle_InitProperties(C4_UI_Rectangle* element, const SDL_FRect destination, const SDL_Color color) {
    if (!element) {
        SDL_Log("Unable to init destination element properties. Pointer is NULL");
        return false;
    }
    element->destination = destination;
    element->color = color;
    return true;
}

C4_UI_Rectangle* C4_UI_Rectangle_Create(const SDL_FRect destination, const SDL_Color color) {
    C4_UI_Rectangle* element = calloc(1, sizeof(C4_UI_Rectangle));
    if (!element) {
        SDL_Log("Unable to allocate memory for rectangle element");
        return NULL;
    }
    if (!C4_UI_Rectangle_InitProperties(element, destination, color)) {
        C4_UI_Rectangle_Destroy(element);
        return NULL;
    }
    return element;
}

void C4_UI_Rectangle_Destroy(void* data) {
    if (!data) {
        SDL_Log("Tried to destroy NULL rectangle element");
        return;
    }
    C4_UI_Rectangle* element = (C4_UI_Rectangle*)data;
    free(element);
}

void C4_UI_Rectangle_Draw(void* data, SDL_Renderer* renderer) {
    if (!data) {
        SDL_Log("Rectangle element is NULL");
        return;
    }
    C4_UI_Rectangle* element = (C4_UI_Rectangle*)data;
    SDL_SetRenderDrawColor(renderer, element->color.r, element->color.g, element->color.b, element->color.a);
    SDL_RenderFillRect(renderer, &element->destination);
}