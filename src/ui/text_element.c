#include "Connect4/ui/text_element.h"
#include "Connect4/constants.h"
#include <stdlib.h>
#include <string.h>

C4_TextUIElement* C4_TextUIElement_Create(SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, SDL_Color color, float destinationX, float destinationY) {
    if (!renderer || !str || ptSize <= 0) {
        return NULL;
    }
    C4_TextUIElement* element = calloc(1, sizeof(C4_TextUIElement));
    if (!element) {
        return NULL;
    }
    element->font = C4_GetFont(font);
    if (!element->font) {
        C4_TextUIElement_Destroy(element);
        return NULL;
    }
    element->ptSize = ptSize;
    element->destination = (SDL_FRect){destinationX, destinationY, 800.f, 0.f};
    element->color = color;
    C4_TextUIElement_ChangeStr(element, str);
    C4_TextUIElement_Refresh(element, renderer);
    return element;
}

void C4_TextUIElement_Destroy(C4_TextUIElement* element) {
    if (!element) {
        return;
    }
    SDL_DestroyTexture(element->texture);
    free(element);
}

void C4_TextUIElement_Refresh(C4_TextUIElement* element, SDL_Renderer* renderer) {
    if (!element) {
        return;
    }
    SDL_DestroyTexture(element->texture);
    TTF_SetFontSize(element->font, element->ptSize);
    SDL_Surface* tempSurface = TTF_RenderText_Blended_Wrapped(element->font, element->str, 0, element->color, 0);
    if (tempSurface) {
        element->texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
        element->destination.w = (float)tempSurface->w;
        element->destination.h = (float)tempSurface->h;
        SDL_DestroySurface(tempSurface);
    } else {
        element->destination.w = 0;
        element->destination.h = 0;
    }
}

void C4_TextUIElement_ChangeStr(C4_TextUIElement* element, const char* newStr) {
    if (!element) {
        return;
    }
    size_t totalSize = sizeof(element->str) / sizeof(element->str[0]);
    snprintf(element->str, totalSize, "%s", newStr);
}

void C4_TextUIElement_Draw(C4_TextUIElement* element, SDL_Renderer* renderer) {
    if (!element || !element->texture || !renderer) {
        return;
    }
    SDL_RenderTexture(renderer, element->texture, NULL, &element->destination);
}

void C4_TextUIElement_CenterInWindow(C4_TextUIElement* element, C4_Axis axis) {
    if (!element) {
        return;
    }
    if (axis == C4_Axis_X || axis == C4_Axis_XY) {
        element->destination.x = (C4_BASE_WINDOW_WIDTH / 2.f) - (element->destination.w / 2.f);
    }
    if (axis == C4_Axis_Y || axis == C4_Axis_XY) {
        element->destination.y = (C4_BASE_WINDOW_HEIGHT / 2.f) - (element->destination.h / 2.f);
    }
}