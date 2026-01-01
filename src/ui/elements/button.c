#include "Connect4/ui/elements/button.h"
#include "Connect4/ui/cursorManager.h"
#include <stdlib.h>
#include <string.h>

static C4_UI_Callback DefaultCallbacks[C4_UI_Button_CallbackType_Length] = {0};
static void* DefaultContexts[C4_UI_Button_CallbackType_Length] = {0};

void C4_UI_Button_CenterElementsInBackground(C4_UI_Button* button, C4_Axis axis) {
    if (!button) {
        SDL_Log("Button is NULL");
        return;
    }
    button->borders.destination.w = button->background.destination.w;
    button->borders.destination.h = button->background.destination.h;
    if (axis == C4_Axis_X || axis == C4_Axis_XY) {
        float backgroundCenterX = button->background.destination.x + (button->background.destination.w / 2.f);
        button->text.destination.x = backgroundCenterX - (button->text.destination.w / 2.f);
        button->borders.destination.x = button->background.destination.x;
        button->symbol.destination.x = backgroundCenterX - (button->symbol.destination.w / 2.f);
    }
    if (axis == C4_Axis_Y || axis == C4_Axis_XY) {
        float backgroundCenterY = button->background.destination.y + (button->background.destination.h / 2.f);
        button->text.destination.y = backgroundCenterY - (button->text.destination.h / 2.f);
        button->borders.destination.y = button->background.destination.y;
        button->symbol.destination.y = backgroundCenterY - (button->symbol.destination.h / 2.f);
    }
}

void C4_UI_Button_SetTheme(C4_UI_Button* button, const C4_UI_Theme* theme) {
    if (!button || !theme) {
        return;
    }
    button->defaultColors = theme->buttonDefault;
    button->hoverColors = theme->buttonHovered;
    button->pressedColors = theme->buttonPressed;
    button->inactiveColors = theme->buttonInactive;
}

bool C4_UI_Button_InitProperties(C4_UI_Button* button, SDL_Renderer* renderer, const C4_UI_Button_Config* config) {
    if (!renderer) {
        SDL_Log("Unable to init button properties. Renderer is NULL");
        return false;
    }
    if (!config) {
        return false;
    }
    if (!config->theme) {
        SDL_Log("Unable to init button properties. Theme is NULL");
        return false;
    }
    const C4_UI_Theme* theme = config->theme;
    C4_UI_Button_SetTheme(button, theme);
    if (
        !C4_UI_Rectangle_InitProperties(
            &button->background,
            &(C4_UI_Rectangle_Config){
                .destination = config->destination,
                .color = button->defaultColors.background
            }
        )
    ) {
        return false;
    }
    if (
        !C4_UI_Text_InitProperties(
            &button->text, renderer,
            &(C4_UI_Text_Config){
                .font = config->font,
                .color = theme->buttonDefault.text,
                .ptSize = theme->defaultPtSize,
                .destinationX = config->destination.x,
                .destinationY = config->destination.y,
                .wrapWidth = 0,
                .str = config->text
            }
        )
    ) {
        return false;
    }
    if (
        !C4_UI_Borders_InitProperties(
            &button->borders,
            &(C4_UI_Borders_Config){
                .destination = config->destination,
                .color = theme->buttonDefault.borders,
                .width = theme->borderWidth
            }
        )
    ) {
        return false;
    }
    if (
        !C4_UI_Symbol_InitProperties(
            &button->symbol, 
            &(C4_UI_Symbol_Config){
                .type = config->symbol,
                .destination = (SDL_FRect){config->destination.x, config->destination.y, 50.f, 50.f},
                .rotationDegrees = 0,
                .color = theme->textColor
            }
        )
    ) {
        return false;
    }
    button->isHovered = false;
    button->isPressed = false;
    button->isActive = true;
    button->resetHoverOnClick = false;
    button->OnClickCallback = DefaultCallbacks[C4_UI_Button_CallbackType_OnClick];
    button->OnClickContext = DefaultContexts[C4_UI_Button_CallbackType_OnClick];
    button->WhilePressedCallback = DefaultCallbacks[C4_UI_Button_CallbackType_WhilePressed];
    button->WhilePressedContext = DefaultContexts[C4_UI_Button_CallbackType_WhilePressed];
    button->OnPressedCallback = DefaultCallbacks[C4_UI_Button_CallbackType_OnPressed];
    button->OnPressedContext = DefaultContexts[C4_UI_Button_CallbackType_OnPressed];
    button->OnHoverCallback = DefaultCallbacks[C4_UI_Button_CallbackType_OnHover];
    button->OnHoverContext = DefaultContexts[C4_UI_Button_CallbackType_OnHover];
    button->interval = 0.1f;
    button->delay = 0.5f;
    button->pressTimer = 0.f;
    button->isRepeating = false;
    C4_UI_Button_CenterElementsInBackground(button, C4_Axis_XY);
    return true;
}

C4_UI_Button* C4_UI_Button_Create(SDL_Renderer* renderer, const C4_UI_Button_Config* config) {
    C4_UI_Button* button = calloc(1, sizeof(C4_UI_Button));
    if (!button) {
        SDL_Log("Unable to allocate memory for button");
        return NULL;
    }
    if (
        !C4_UI_Button_InitProperties(button, renderer, config)
    ) {
        C4_UI_Button_Destroy(button);
        return NULL;
    }
    return button;
}

void C4_UI_Button_FreeResources(C4_UI_Button* button) {
    C4_UI_Text_FreeResources(&button->text);
}

void C4_UI_Button_Destroy(void* data) {
    if (!data) {
        SDL_Log("Tried to destroy NULL button");
        return;
    }
    C4_UI_Button* button = (C4_UI_Button*)data;
    C4_UI_Button_FreeResources(button);
    free(button);
}

void C4_UI_Button_Draw(void* data, SDL_Renderer* renderer) {
    if (!data) {
        SDL_Log("Button is NULL");
        return;
    }
    C4_UI_Button* button = (C4_UI_Button*)data;
    C4_UI_Button_ColorInfo* currentColors = &button->defaultColors;
    if (button->isPressed) {
        currentColors = &button->pressedColors;
    } else if (button->isHovered) {
        currentColors = &button->hoverColors;
    }
    if (!button->isActive) {
        currentColors = &button->inactiveColors;
    }
    button->background.color = currentColors->background;
    C4_UI_Rectangle_Draw(&button->background, renderer);
    if (button->borders.width > 0) {
        button->borders.color = currentColors->borders;
        C4_UI_Borders_Draw(&button->borders, renderer);
    }
    if (button->symbol.type != C4_UI_SymbolType_None) {
        button->symbol.color = currentColors->symbol;
        C4_UI_Symbol_Draw(&button->symbol, renderer);
    }
    if (button->text.str[0] != '\0') {
        button->text.color = currentColors->text;
        C4_UI_Text_Draw(&button->text, renderer);
    }
}

void C4_UI_Button_Update(void* data, float deltaTime) {
    if (!data) {
        return;
    }
    C4_UI_Button* button = (C4_UI_Button*)data;
    if (!button->isActive) {
        button->isPressed = false;
        button->isHovered = false;
        button->isRepeating = false;
        button->pressTimer = 0.f;
    }
    if (!button || !button->isPressed || !button->WhilePressedCallback) {
        button->pressTimer = 0.f;
        button->isRepeating = false;
        return;
    }
    button->pressTimer += deltaTime;
    if (!button->isRepeating) {
        if (button->pressTimer >= button->delay) {
            button->WhilePressedCallback(button->WhilePressedContext);
            button->pressTimer = 0.f;
            button->isRepeating = true;
        }
    } else {
        if (button->pressTimer >= button->interval) {
            button->WhilePressedCallback(button->WhilePressedContext);
            button->pressTimer = 0.f;
        }
    }
}

void C4_UI_Button_HandleMouseEvents(void* data, SDL_Event* event) {
    if (!data) {
        return;
    }
    C4_UI_Button* button = (C4_UI_Button*)data;
    if (!event) {
        SDL_Log("Event is NULL");
        return;
    }
    if (!button->isActive) {
        return;
    }
    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        bool currentlyHovered = (
            event->motion.x >= button->background.destination.x &&
            event->motion.x <= button->background.destination.x + button->background.destination.w &&
            event->motion.y >= button->background.destination.y &&
            event->motion.y <= button->background.destination.y + button->background.destination.h
        );
        if (currentlyHovered != button->isHovered) {
            button->isHovered = currentlyHovered;
            SDL_SetCursor(C4_GetSystemCursor(currentlyHovered ? SDL_SYSTEM_CURSOR_POINTER : SDL_SYSTEM_CURSOR_DEFAULT));
            if (button->isHovered && button->OnHoverCallback) {
                button->OnHoverCallback(button->OnHoverContext);
            }
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event->button.button == SDL_BUTTON_LEFT && button->isHovered) {
            button->isPressed = true;
            if (button->OnPressedCallback) {
                button->OnPressedCallback(button->OnPressedContext);
            }
            if (button->WhilePressedCallback) {
                button->WhilePressedCallback(button->WhilePressedContext);
            }
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            bool wasClicked = button->isPressed && button->isHovered;
            button->isPressed = false;
            if (wasClicked && button->OnClickCallback)  {
                button->OnClickCallback(button->OnClickContext);
                if (button->resetHoverOnClick) {
                    button->isPressed = false;
                }
            }
        }
    }
}

void C4_UI_Button_CenterInWindow(C4_UI_Button* button, C4_Axis axis, unsigned int windowWidth, unsigned int windowHeight) {
    if (!button) {
        SDL_Log("Button is NULL");
        return;
    }
    C4_UI_CenterInWindow(&button->background.destination, axis, windowWidth, windowHeight);
    C4_UI_Button_CenterElementsInBackground(button, axis);
}

void C4_UI_Button_TransformResize(C4_UI_Button* button, float x, float y, float w, float h) {
    if (!button) {
        SDL_Log("Tried to resize NULL button");
        return;
    }
    button->background.destination = (SDL_FRect){x, y, w, h};
    C4_UI_Button_CenterElementsInBackground(button, C4_Axis_XY);
}

void C4_UI_Button_UpdateStr(C4_UI_Button* button, const char* str, SDL_Renderer* renderer) {
    if (!button || !str) {
        return;
    }
    if (strcmp(button->text.str, str) == 0) {
        return;
    }
    C4_UI_Text_UpdateStr(&button->text, str, renderer);
    float backgroundCenterX = button->background.destination.x + (button->background.destination.w / 2.f);
    button->text.destination.x = backgroundCenterX - (button->text.destination.w / 2.f);
    float backgroundCenterY = button->background.destination.y + (button->background.destination.h / 2.f);
    button->text.destination.y = backgroundCenterY - (button->text.destination.h / 2.f);
}

void C4_UI_Button_Reset(void* data) {
    if (!data) {
        return;
    }
    C4_UI_Button* button = (C4_UI_Button*)data;
    button->isHovered = false;
    button->isPressed = false;
}

void C4_UI_Button_SetDefaultCallback(C4_UI_Button_CallbackType type, C4_UI_Callback callback, void* context) {
    if (type < C4_UI_Button_CallbackType_OnClick || type >= C4_UI_Button_CallbackType_Length) {
        return;
    }
    DefaultCallbacks[type] = callback;
    DefaultContexts[type] = context;
}