#include "Connect4/ui/elements/text.h"
#include "Connect4/constants.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

bool C4_UI_Text_InitProperties(C4_UI_Text* element, SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, float destinationX, float destinationY, int wrapWidth, const SDL_Color color) {
    if (!element) {
        SDL_Log("Unable to init text element properties. Pointer is NULL");
        return false;
    }
    if (!renderer) {
        SDL_Log("Unable to init text element properties. renderer is NULL.");
        return false;
    }
    if (!str) {
        SDL_Log("Unable to init text element properties. str is NULL");
        return false;
    }
    element->font = C4_GetFont(font);
    if (!element->font) {
        SDL_Log("Unable to get text element font");
        return false;
    }
    if (ptSize <= 0) {
        SDL_Log("Unable to create text element. ptSize must be greater than 0.");
        return false;
    }
    element->ptSize = ptSize;
    element->destination = (SDL_FRect){destinationX, destinationY, 0.f, 0.f};
    element->color = color;
    element->wrapWidth = wrapWidth;
    C4_UI_Text_UpdateStr(element, str, renderer);
    return true;
}

C4_UI_Text* C4_UI_Text_Create(SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, float destinationX, float destinationY, int wrapWidth, const SDL_Color color) {
    C4_UI_Text* element = calloc(1, sizeof(C4_UI_Text));
    if (!element) {
        SDL_Log("Unable to allocate memory for text element");
        return NULL;
    }
    if (!C4_UI_Text_InitProperties(element, renderer, str, font, ptSize, destinationX, destinationY, wrapWidth, color)) {
        C4_UI_Text_Destroy(element);
        return NULL;
    }
    return element;
}

void C4_UI_Text_FreeResources(C4_UI_Text* element) {
    if (element->texture) {
        SDL_DestroyTexture(element->texture);
        element->texture = NULL;
    }
}

void C4_UI_Text_Destroy(C4_UI_Text* element) {
    if (!element) {
        SDL_Log("Tried to destroy NULL text element");
        return;
    }
    C4_UI_Text_FreeResources(element);
    free(element);
}

void C4_UI_Text_ReloadTexture(C4_UI_Text* element, SDL_Renderer* renderer) {
    if (!element) {
        SDL_Log("Text element is NULL");
        return;
    }
    SDL_DestroyTexture(element->texture);
    size_t strLength = strlen(element->str);
    if (strLength == 0) {
        return;
    } else if (strLength >= C4_UI_TEXT_STR_SIZE) {
        return;
    }
    TTF_SetFontSize(element->font, element->ptSize);
    SDL_Surface* tempSurface = TTF_RenderText_Blended_Wrapped(element->font, element->str, strlen(element->str), (SDL_Color){255, 255, 255, 255}, element->wrapWidth);
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

void C4_UI_Text_UpdateStr(C4_UI_Text* element, const char* newStr, SDL_Renderer* renderer) {
    if (!element) {
        SDL_Log("Text element is NULL");
        return;
    }
    if (strlen(newStr) >= C4_UI_TEXT_STR_SIZE) {
        SDL_Log("Unable to update text element str. Length exceeds max size");
        return;
    }
    snprintf(element->str, sizeof(element->str), "%s", newStr);
    C4_UI_Text_ReloadTexture(element, renderer);
}

void C4_UI_Text_Draw(C4_UI_Text* element, SDL_Renderer* renderer) {
    if (!element) {
        SDL_Log("Text element is NULL");
        return;
    }
    if (!element->texture) {
        // Doesnt need a log since this would be true if the text element is currently an empty string
        return;
    }
    if (!renderer) {
        SDL_Log("Text element renderer is NULL");
        return;
    }
    if (element->str[0] == '\0') {
        return;
    }
    SDL_SetTextureColorMod(element->texture, element->color.r, element->color.g, element->color.b);
    SDL_SetTextureAlphaMod(element->texture, element->color.a);
    SDL_RenderTexture(renderer, element->texture, NULL, &element->destination);
}