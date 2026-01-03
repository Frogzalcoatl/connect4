#include "Connect4/ui/elements/rectangle.h"
#include <stdlib.h>

bool C4_UI_Rectangle_InitProperties(C4_UI_Rectangle* element, const C4_UI_Rectangle_Config* config) {
    if (!element) {
        SDL_Log("Unable to init destination element properties. Pointer is NULL");
        return false;
    }
    if (!config) {
        return false;
    }
    element->destination = config->destination;
    element->color = config->color;
    return true;
}

C4_UI_Rectangle* C4_UI_Rectangle_Create(const C4_UI_Rectangle_Config* config) {
    C4_UI_Rectangle* element = calloc(1, sizeof(C4_UI_Rectangle));
    if (!element) {
        SDL_Log("Unable to allocate memory for rectangle element");
        return NULL;
    }
    if (!C4_UI_Rectangle_InitProperties(element, config)) {
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

void C4_UI_Rectangle_Draw(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY) {
    if (!data) {
        SDL_Log("Rectangle element is NULL");
        return;
    }
    C4_UI_Rectangle* element = (C4_UI_Rectangle*)data;

    SDL_FRect drawRect;
    C4_UI_GetScaledRect(&element->destination, &drawRect, scale, parentX, parentY);

    SDL_SetRenderDrawColor(renderer, element->color.r, element->color.g, element->color.b, element->color.a);
    SDL_RenderFillRect(renderer, &drawRect);
}