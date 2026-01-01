#include "Connect4/ui/elements/borders.h"
#include <stdlib.h>

bool C4_UI_Borders_InitProperties(C4_UI_Borders* element, const C4_UI_Borders_Config* config) {
    if (!element) {
        SDL_Log("Unable to init properties for border element. Pointer is NULL");
        return false;
    }
    if (!config) {
        return false;
    }
    element->destination = config->destination;
    element->color = config->color;
    element->width = config->width;
    return true;
}

C4_UI_Borders* C4_UI_Borders_Create(const C4_UI_Borders_Config* config) {
    C4_UI_Borders* element = calloc(1, sizeof(C4_UI_Borders));
    if (!element) {
        SDL_Log("Unable to allocate memory for borders element");
        return NULL;
    }
    if (!C4_UI_Borders_InitProperties(element, config)) {
        C4_UI_Borders_Destroy(element);
        return NULL;
    }
    return element;
}

void C4_UI_Borders_Destroy(void* data) {
    if (!data) {
        SDL_Log("Tried to destroy NULL borders element");
        return;
    }
    C4_UI_Borders* element = (C4_UI_Borders*)data;
    free(element);
}

void C4_UI_Borders_Draw(void* data, SDL_Renderer* renderer) {
    if (!data) {
        SDL_Log("Unable to draw Borders element. Pointer is NULL");
        return;
    }
    C4_UI_Borders* element = (C4_UI_Borders*)data;
    if (element->width == 0) {
        return;
    }
    SDL_SetRenderDrawColor(renderer, element->color.r, element->color.g, element->color.b, element->color.a);
    SDL_FRect topBorder = {element->destination.x, element->destination.y, element->destination.w, (float)element->width};
    SDL_FRect bottomBorder = {element->destination.x, element->destination.y + element->destination.h - (float)element->width, element->destination.w, (float)element->width};
    SDL_FRect leftBorder = {element->destination.x, element->destination.y, (float)element->width, element->destination.h};
    SDL_FRect rightBorder = {element->destination.x + element->destination.w - (float)element->width, element->destination.y, (float)element->width, element->destination.h};
    SDL_RenderFillRect(renderer, &topBorder);
    SDL_RenderFillRect(renderer, &bottomBorder);
    SDL_RenderFillRect(renderer, &leftBorder);
    SDL_RenderFillRect(renderer, &rightBorder);
}