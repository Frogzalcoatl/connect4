#include "Connect4/ui/elements/numberInput.h"
#include "Connect4/connect4.h"
#include "Connect4/constants.h"
#include "Connect4/game/utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static void C4_UI_NumberInput_PositionElementsInBackground(C4_UI_NumberInput* numInput) {
    if (!numInput) {
        return;
    }
    const SDL_FRect* dest = &numInput->background.destination;
    const unsigned int borderWidth = numInput->borders.width;
    numInput->borders.destination = *dest;
    numInput->numberText.destination.x = dest->x + borderWidth + 10;
    numInput->numberText.destination.y = dest->y + (dest->h / 2.f) - (numInput->numberText.destination.h / 2.f);
    C4_UI_ButtonGroup_TransformResize(
        &numInput->arrows,
        (SDL_FRect){
            .x = dest->x + dest->w * 0.75f,
            .y = dest->y,
            .w = dest->w / 4.f,
            .h = dest->h
        }
    );
}

bool C4_UI_NumberInput_InitProperties(
    C4_UI_NumberInput* numInput, SDL_Renderer* renderer, const SDL_FRect destination, unsigned int min, unsigned int max,
    unsigned int startingValue, float arrowWidth, float arrowHeight, const C4_UI_Theme* theme
) {
    if (!numInput) {
        SDL_Log("Unable to init number input element. Pointer is NULL");
        return false;
    }
    if (min > max) {
        SDL_Log("Unable to init number input element. Min should be less than max");
        return false;
    }
    if (startingValue < min || startingValue > max) {
        SDL_Log("Unable to init number input element. Starting value should be within min/max");
        return false;
    }
    if (!renderer) {
        SDL_Log("Unable to init number input element. Renderer is NULL");
        return false;
    }
    if (!theme) {
        SDL_Log("Unable to init number input element. Theme is NULL");
        return false;
    }
    if (!C4_UI_Borders_InitProperties(&numInput->borders, destination, theme->buttonDefault.borders, theme->borderWidth)) {
        return false;
    }
    if (!C4_UI_Rectangle_InitProperties(&numInput->background, destination, (SDL_Color){0, 0, 0, 255})) {
        return false;
    }
    if (
        !C4_UI_ButtonGroup_InitProperties(
            &numInput->arrows, renderer, (SDL_FRect){0.f, 0.f, arrowWidth, destination.h / 2.f},
            2, C4_UI_ButtonGroup_Direction_Vertical, 0, theme
        )
    ) {
        return false;
    }
    char startingValueStr[64];
    snprintf(startingValueStr, sizeof(startingValueStr), "%d", startingValue);
    C4_UI_ButtonGroup_SetButtonIndex(&numInput->arrows, 0, renderer, "", C4_UI_SymbolType_Triangle, arrowWidth, arrowHeight, 0, theme);
    C4_UI_ButtonGroup_SetButtonIndex(&numInput->arrows, 1, renderer, "", C4_UI_SymbolType_Triangle, arrowWidth, arrowHeight, 180, theme);
    float textPtSize = roundf(destination.h / 1.25f);
    if (!C4_UI_Text_InitProperties(&numInput->numberText, renderer, startingValueStr, C4_FontType_Regular, textPtSize, 0.f, 0.f, 0, theme->textColor)) {
        return false;
    }
    numInput->min = min;
    numInput->max = max;
    numInput->currentValue = startingValue;
    C4_UI_NumberInput_PositionElementsInBackground(numInput);
    return true;
}

C4_UI_NumberInput* C4_UI_NumberInput_Create(
    SDL_Renderer* renderer, const SDL_FRect destination, unsigned int min, unsigned int max,
    unsigned int startingValue, float arrowWidth, float arrowHeight, const C4_UI_Theme* theme
) {
    C4_UI_NumberInput* numInput = calloc(1, sizeof(C4_UI_NumberInput));
    if (!numInput) {
        SDL_Log("Unable to allocate memory for number input element");
        return NULL;
    }
    if (
        !C4_UI_NumberInput_InitProperties(
            numInput, renderer, destination, min, max, startingValue, arrowWidth, arrowHeight, theme
        )
    ) {
        return NULL;
    }
    return numInput;
}

void C4_UI_NumberInput_FreeResources(C4_UI_NumberInput* numInput) {
    if (!numInput) {
        return;
    }
    C4_UI_Text_FreeResources(&numInput->numberText);
    C4_UI_ButtonGroup_FreeResources(&numInput->arrows);
}

void C4_UI_NumberInput_Destroy(C4_UI_NumberInput* numInput) {
    if (!numInput) {
        return;
    }
    C4_UI_NumberInput_FreeResources(numInput);
    free(numInput);
}

void C4_UI_NumberInput_Draw(C4_UI_NumberInput* numInput, SDL_Renderer* renderer) {
    if (!numInput) {
        SDL_Log("Unable to draw number input element. Pointer is NULL");
        return;
    }
    if (!renderer) {
        SDL_Log("Unable to draw number input element. Renderer is NULL");
        return;
    }
    C4_UI_Rectangle_Draw(&numInput->background, renderer);
    C4_UI_Text_Draw(&numInput->numberText, renderer);
    C4_UI_Borders_Draw(&numInput->borders, renderer);
    C4_UI_ButtonGroup_Draw(&numInput->arrows, renderer);
}

void C4_UI_NumberInput_CenterInWindow(C4_UI_NumberInput* numInput, C4_Axis axis) {
    if (!numInput) {
        return;
    }
    C4_UI_CenterInWindow(&numInput->background.destination, axis);
    C4_UI_NumberInput_PositionElementsInBackground(numInput);
}

void C4_UI_NumberInput_HandleMouseEvents(C4_UI_NumberInput* numInput, SDL_Event* event, SDL_Renderer* renderer) {
    if (!numInput) {
        return;
    }
    switch(C4_UI_ButtonGroup_HandleMouseEvents(&numInput->arrows, event)) {
        // Up Button
        case 0: {
            if (numInput->currentValue + 1 <= numInput->max) {
                numInput->currentValue++;
                snprintf(numInput->numberText.str, sizeof(numInput->numberText.str), "%d", numInput->currentValue);
                C4_UI_Text_ReloadTexture(&numInput->numberText, renderer);
            }
        }; break;
        // Down Button
        case 1: {
            if (numInput->currentValue - 1 >= numInput->min) {
                numInput->currentValue--;
                snprintf(numInput->numberText.str, sizeof(numInput->numberText.str), "%d", numInput->currentValue);
                C4_UI_Text_ReloadTexture(&numInput->numberText, renderer);
            }
        }; break;
    }
}

void C4_UI_NumberInput_HandleKeyboardInput(C4_UI_NumberInput* numInput, SDL_Event* event, SDL_Renderer* renderer) {
    
}

void C4_UI_NumberInput_ChangeMax(C4_UI_NumberInput* numInput, unsigned int newMax, SDL_Renderer* renderer) {
    if (!numInput) {
        return;
    }
    if (newMax < numInput->min) {
        return;
    }
    numInput->max = newMax;
    if (numInput->currentValue > newMax) {
        numInput->currentValue = newMax;
        snprintf(numInput->numberText.str, sizeof(numInput->numberText.str), "%d", numInput->currentValue);
        C4_UI_Text_ReloadTexture(&numInput->numberText, renderer);
    }
}

void C4_UI_NumberInput_ChangeMin(C4_UI_NumberInput* numInput, unsigned int newMin, SDL_Renderer* renderer) {
    if (!numInput) {
        return;
    }
    if (newMin > numInput->max) {
        return;
    }
    numInput->max = newMin;
    if (numInput->currentValue < newMin) {
        numInput->currentValue = newMin;
        snprintf(numInput->numberText.str, sizeof(numInput->numberText.str), "%d", numInput->currentValue);
        C4_UI_Text_ReloadTexture(&numInput->numberText, renderer);
    }
}