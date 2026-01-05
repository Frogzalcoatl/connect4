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

void C4_UI_Borders_Draw(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY) {
    if (!data) {
        SDL_Log("Unable to draw Borders element. Pointer is NULL");
        return;
    }
    C4_UI_Borders* element = (C4_UI_Borders*)data;
    if (element->width == 0) {
        return;
    }

    SDL_FRect drawRect;
    C4_UI_GetScaledRect(&element->destination, &drawRect, scale, parentX, parentY);
    
    float scaledBorderWidth = (float)element->width * scale;

    SDL_FRect topBorder = {drawRect.x, drawRect.y, drawRect.w, scaledBorderWidth};
    SDL_FRect bottomBorder = {drawRect.x, drawRect.y + drawRect.h - scaledBorderWidth, drawRect.w, scaledBorderWidth};
    SDL_FRect leftBorder = {drawRect.x, drawRect.y, scaledBorderWidth, drawRect.h};
    SDL_FRect rightBorder = {drawRect.x + drawRect.w - scaledBorderWidth, drawRect.y, scaledBorderWidth, drawRect.h};

    SDL_SetRenderDrawColor(renderer, element->color.r, element->color.g, element->color.b, element->color.a);

    SDL_RenderFillRect(renderer, &topBorder);
    SDL_RenderFillRect(renderer, &bottomBorder);
    SDL_RenderFillRect(renderer, &leftBorder);
    SDL_RenderFillRect(renderer, &rightBorder);
}

void C4_UI_Borders_ChangeDestination(C4_UI_Borders* element, const SDL_FRect destination) {
    if (!element) {
        return;
    }
    element->destination = destination;
}