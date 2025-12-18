#include "Connect4/ui/elements/borders.h"
#include "Connect4/constants.h"
#include <stdlib.h>

bool C4_UI_Borders_InitProperties(C4_UI_Borders* element, const SDL_FRect destination, const SDL_Color color, const unsigned int width) {
    if (!element) {
        SDL_Log("Unable to init properties for border element. Pointer is NULL");
        return false;
    }
    element->destination = destination;
    element->color = color;
    element->width = width;
    return true;
}

C4_UI_Borders* C4_UI_Borders_Create(const SDL_FRect destination, const SDL_Color color, const unsigned int width) {
    C4_UI_Borders* element = calloc(1, sizeof(C4_UI_Borders));
    if (!element) {
        SDL_Log("Unable to allocate memory for borders element");
        return NULL;
    }
    if (!C4_UI_Borders_InitProperties(element, destination, color, width)) {
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
    SDL_FRect topBorder = {element->destination.x, element->destination.y, element->destination.w, (float)element->width};
    SDL_FRect bottomBorder = {element->destination.x, element->destination.y + element->destination.h - (float)element->width, element->destination.w, (float)element->width};
    SDL_FRect leftBorder = {element->destination.x, element->destination.y, (float)element->width, element->destination.h};
    SDL_FRect rightBorder = {element->destination.x + element->destination.w - (float)element->width, element->destination.y, (float)element->width, element->destination.h};
    SDL_RenderFillRect(renderer, &topBorder);
    SDL_RenderFillRect(renderer, &bottomBorder);
    SDL_RenderFillRect(renderer, &leftBorder);
    SDL_RenderFillRect(renderer, &rightBorder);
}