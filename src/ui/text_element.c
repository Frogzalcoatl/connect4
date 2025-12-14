#include "Connect4/ui/text_element.h"
#include "Connect4/constants.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

C4_UI_Text* C4_UI_Text_Create(SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, SDL_Color color, float destinationX, float destinationY) {
    if (!renderer || !str) {
        SDL_Log("Unable to create text element. renderer and/or str is NULL.");
        return NULL;
    }
    if (ptSize <= 0) {
        SDL_Log("Unable to create text element. ptSize must be greater than 0.");
        return NULL;
    }
    C4_UI_Text* element = calloc(1, sizeof(C4_UI_Text));
    if (!element) {
        SDL_Log("Unable to allocate memory for text element");
        return NULL;
    }
    element->font = C4_GetFont(font);
    if (!element->font) {
        SDL_Log("Unable to get text element font");
        C4_UI_Text_Destroy(element);
        return NULL;
    }
    element->ptSize = ptSize;
    element->destination = (SDL_FRect){destinationX, destinationY, 0.f, 0.f};
    element->color = color;
    C4_UI_Text_ChangeStr(element, str);
    C4_UI_Text_Refresh(element, renderer);
    return element;
}

void C4_UI_Text_Destroy(C4_UI_Text* element) {
    if (!element) {
        SDL_Log("Tried to destroy NULL text element");
        return;
    }
    SDL_DestroyTexture(element->texture);
    free(element);
}

void C4_UI_Text_Refresh(C4_UI_Text* element, SDL_Renderer* renderer) {
    if (!element) {
        SDL_Log("Text element is NULL");
        return;
    }
    SDL_DestroyTexture(element->texture);
    TTF_SetFontSize(element->font, element->ptSize);
    SDL_Surface* tempSurface = TTF_RenderText_Blended_Wrapped(element->font, element->str, 0, (SDL_Color){255, 255, 255, 255}, 0);
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

void C4_UI_Text_ChangeStr(C4_UI_Text* element, const char* newStr) {
    if (!element) {
        SDL_Log("Text element is NULL");
        return;
    }
    size_t totalSize = sizeof(element->str) / sizeof(element->str[0]);
    snprintf(element->str, totalSize, "%s", newStr);
}

void C4_UI_Text_Draw(C4_UI_Text* element, SDL_Renderer* renderer) {
    if (!element || !element->texture || !renderer) {
        SDL_Log("Text element, text element texture, and/or renderer is NULL");
        return;
    }
    SDL_SetTextureColorMod(element->texture, element->color.r, element->color.g, element->color.b);
    SDL_SetTextureAlphaMod(element->texture, element->color.a);
    SDL_RenderTexture(renderer, element->texture, NULL, &element->destination);
}

void C4_UI_Text_CenterInWindow(C4_UI_Text* element, C4_Axis axis) {
    if (!element) {
        SDL_Log("Text element is NULL");
        return;
    }
    if (axis == C4_Axis_X || axis == C4_Axis_XY) {
        element->destination.x = (C4_BASE_WINDOW_WIDTH / 2.f) - (element->destination.w / 2.f);
    }
    if (axis == C4_Axis_Y || axis == C4_Axis_XY) {
        element->destination.y = (C4_BASE_WINDOW_HEIGHT / 2.f) - (element->destination.h / 2.f);
    }
}