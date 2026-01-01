#include "Connect4/ui/elements/numberInput.h"
#include "Connect4/ui/utils.h"
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
        &numInput->buttonGroup,
        (SDL_FRect){
            .x = dest->x + dest->w * 0.75f,
            .y = dest->y,
            .w = dest->w / 4.f,
            .h = dest->h
        }
    );
}

// Temporary until i improve text rendering is improved to not require the renderer for every text update
static SDL_Renderer* temporaryGlobalRenderer;

void C4_UI_NumberInput_GenericIncrementCallback(void* numberInputContext) {
    C4_UI_NumberInput* numInput = (C4_UI_NumberInput*)numberInputContext;
    if (numInput->currentValue < numInput->max) {
        numInput->currentValue++;
        snprintf(numInput->numberText.str, sizeof(numInput->numberText.str), "%d", numInput->currentValue);
        C4_UI_Text_ReloadTexture(&numInput->numberText, temporaryGlobalRenderer);
    }
}

void C4_UI_NumberInput_GenericDecrementCallback(void* numberInputContext) {
    C4_UI_NumberInput* numInput = (C4_UI_NumberInput*)numberInputContext;
    if (numInput->currentValue > numInput->min) {
        numInput->currentValue--;
        snprintf(numInput->numberText.str, sizeof(numInput->numberText.str), "%d", numInput->currentValue);
        C4_UI_Text_ReloadTexture(&numInput->numberText, temporaryGlobalRenderer);
    }
}

bool C4_UI_NumberInput_InitProperties(
    C4_UI_NumberInput* numInput, SDL_Renderer* renderer, const C4_UI_NumberInput_Config* config
) {
    temporaryGlobalRenderer = renderer;
    if (!numInput) {
        SDL_Log("Unable to init number input element. Pointer is NULL");
        return false;
    }
    if (!config) {
        return false;
    }
    if (!config->theme) {
        return false;
    }
    if (config->min > config->max) {
        SDL_Log("Unable to init number input element. Min should be less than max");
        return false;
    }
    if (config->startingValue < config->min || config->startingValue > config->max) {
        SDL_Log("Unable to init number input element. Starting value should be within min/max");
        return false;
    }
    if (!renderer) {
        SDL_Log("Unable to init number input element. Renderer is NULL");
        return false;
    }
    if (
        !C4_UI_Borders_InitProperties(
            &numInput->borders,
            &(C4_UI_Borders_Config){
                .destination = config->destination,
                .color = config->theme->buttonDefault.borders,
                .width = config->theme->borderWidth
            }
        )
    ) {
        return false;
    }
    if (
        !C4_UI_Rectangle_InitProperties(
            &numInput->background,
            &(C4_UI_Rectangle_Config){
                .destination = config->destination,
                .color = (SDL_Color){0, 0, 0, 255}
            }
        )
    ) {
        return false;
    }
    if (
        !C4_UI_ButtonGroup_InitProperties(
            &numInput->buttonGroup, renderer,
            &(C4_UI_ButtonGroup_Config){
                .destination = config->destination,
                .count = 2,
                .buttonDirection = C4_UI_ButtonGroup_Direction_Vertical,
                .margin = 0,
                .font = config->font,
                .theme = config->theme
            }
        )
    ) {
        return false;
    }
    C4_UI_Button* btnIncrement = &numInput->buttonGroup.buttons[0];
    btnIncrement->OnClickCallback = C4_UI_NumberInput_GenericIncrementCallback;
    btnIncrement->OnClickContext = numInput;
    btnIncrement->WhilePressedCallback = C4_UI_NumberInput_GenericIncrementCallback;
    btnIncrement->WhilePressedContext = numInput;
    btnIncrement->symbol.type = C4_UI_SymbolType_Triangle;
    btnIncrement->symbol.destination.w = 20.f;
    btnIncrement->symbol.destination.h = 20.f;
    C4_UI_Button* btnDecrement = &numInput->buttonGroup.buttons[1];
    btnDecrement->OnClickCallback = C4_UI_NumberInput_GenericDecrementCallback;
    btnDecrement->OnClickContext = numInput;
    btnDecrement->WhilePressedCallback = C4_UI_NumberInput_GenericDecrementCallback;
    btnDecrement->WhilePressedContext = numInput;
    btnDecrement->symbol.type = C4_UI_SymbolType_Triangle;
    btnDecrement->symbol.destination.w = 20.f;
    btnDecrement->symbol.destination.h = 20.f;
    btnDecrement->symbol.rotationDegrees = 180;
    float textPtSize = roundf(config->destination.h / 1.25f);
    char startingValueStr[64];
    snprintf(startingValueStr, sizeof(startingValueStr), "%d", config->startingValue);
    if (
        !C4_UI_Text_InitProperties(
            &numInput->numberText, renderer,
            &(C4_UI_Text_Config){
                .str = startingValueStr,
                .font = config->font,
                .color = config->theme->textColor,
                .ptSize = textPtSize,
                .destinationX = 0.f,
                .destinationY = 0.f,
                .wrapWidth = 0
            }
        )
    ) {
        return false;
    }
    numInput->min = config->min;
    numInput->max = config->max;
    numInput->currentValue = config->startingValue;
    C4_UI_NumberInput_PositionElementsInBackground(numInput);
    return true;
}

C4_UI_NumberInput* C4_UI_NumberInput_Create(
    SDL_Renderer* renderer, const C4_UI_NumberInput_Config* config
) {
    C4_UI_NumberInput* numInput = calloc(1, sizeof(C4_UI_NumberInput));
    if (!numInput) {
        SDL_Log("Unable to allocate memory for number input element");
        return NULL;
    }
    if (
        !C4_UI_NumberInput_InitProperties(numInput, renderer, config)
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
    C4_UI_ButtonGroup_FreeResources(&numInput->buttonGroup);
}

void C4_UI_NumberInput_Destroy(void* data) {
    if (!data) {
        return;
    }
    C4_UI_NumberInput* numInput = (C4_UI_NumberInput*)data;
    C4_UI_NumberInput_FreeResources(numInput);
    free(numInput);
}

void C4_UI_NumberInput_Draw(void* data, SDL_Renderer* renderer) {
    if (!data) {
        return;
    }
    C4_UI_NumberInput* numInput = (C4_UI_NumberInput*)data;
    if (!renderer) {
        SDL_Log("Unable to draw number input element. Renderer is NULL");
        return;
    }
    C4_UI_Rectangle_Draw(&numInput->background, renderer);
    C4_UI_Text_Draw(&numInput->numberText, renderer);
    C4_UI_Borders_Draw(&numInput->borders, renderer);
    C4_UI_ButtonGroup_Draw(&numInput->buttonGroup, renderer);
}

void C4_UI_NumberInput_CenterInWindow(C4_UI_NumberInput* numInput, C4_Axis axis, unsigned int windowWidth, unsigned int windowHeight) {
    if (!numInput) {
        return;
    }
    C4_UI_CenterInWindow(&numInput->background.destination, axis, windowWidth, windowHeight);
    C4_UI_NumberInput_PositionElementsInBackground(numInput);
}

void C4_UI_NumberInput_Update(void* data, float deltaTime) {
    if (!data) {
        return;
    }
    C4_UI_NumberInput* numInput = (C4_UI_NumberInput*)data;
    C4_UI_ButtonGroup_Update(&numInput->buttonGroup, deltaTime);
}

void C4_UI_NumberInput_HandleMouseEvents(void* data, SDL_Event* event) {
    if (!data) {
        return;
    }
    C4_UI_NumberInput* numInput = (C4_UI_NumberInput*)data;
    C4_UI_ButtonGroup_HandleMouseEvents(&numInput->buttonGroup, event);
}

void C4_UI_NumberInput_HandleKeyboardInput(C4_UI_NumberInput* numInput, SDL_Event* event, SDL_Renderer* renderer) {
    (void)numInput; (void)event; (void)renderer;
    
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

void C4_UI_NumberInput_ResetButtons(void* data) {
    if (!data) {
        return;
    }
    C4_UI_NumberInput* numInput = (C4_UI_NumberInput*)data;
    C4_UI_ButtonGroup_Reset(&numInput->buttonGroup);
}