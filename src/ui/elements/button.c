#include "Connect4/ui/elements/button.h"
#include "Connect4/ui/cursorManager.h"
#include "Connect4/assets/sounds.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include <stdlib.h>
#include <string.h>

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

bool C4_UI_Button_InitProperties(
    C4_UI_Button* button, SDL_Renderer* renderer, const char* str, const SDL_FRect destination,
    C4_UI_SymbolType symbol, float symbolWidth, float symbolHeight, int symbolRotationDegrees, const C4_UI_Theme* theme
) {
    if (!renderer) {
        SDL_Log("Unable to init button properties. Renderer is NULL");
        return false;
    }
    if (!theme) {
        SDL_Log("Unable to init button properties. Theme is NULL");
    }
    button->defaultColors = (C4_UI_ButtonColorInfo){
        theme->buttonDefault.background, theme->buttonDefault.text, theme->buttonDefault.borders, theme->buttonDefault.symbol
    };
    button->hoverColors = (C4_UI_ButtonColorInfo){
        theme->buttonHovered.background, theme->buttonHovered.text, theme->buttonHovered.borders, theme->buttonHovered.symbol
    };
    button->clickColors = (C4_UI_ButtonColorInfo){
        theme->buttonPressed.background, theme->buttonPressed.text, theme->buttonPressed.borders, theme->buttonPressed.symbol
    };
    if (!C4_UI_Rectangle_InitProperties(&button->background, destination, button->defaultColors.background)) {
        return false;
    }
    if (str) {
        if (!C4_UI_Text_InitProperties(&button->text, renderer, str, theme->buttonFont, theme->defaultPtSize, destination.x, destination.y, 0, theme->textColor)) {
            return false;
        }
    } else {
        button->text.str[0] = '\0';
    }
    if (theme->borderWidth > 0) {
        if (!C4_UI_Borders_InitProperties(&button->borders, destination, button->defaultColors.borders, theme->borderWidth)) {
            return false;
        }
    } else {
        button->borders.width = 0;
        button->borders.destination.w = destination.w;
        button->borders.destination.h = destination.h;
    }
    if (symbol != C4_UI_SymbolType_None) {
        if (
            !C4_UI_Symbol_InitProperties(
                &button->symbol, symbol,
                (SDL_FRect){destination.x, destination.y, symbolWidth, symbolHeight},
                symbolRotationDegrees, theme->textColor
            )
        ) {
            return false;
        }
    }
    button->isHovered = false;
    button->isPressed = false;
    C4_UI_Button_CenterElementsInBackground(button, C4_Axis_XY);
    return true;
}

C4_UI_Button* C4_UI_Button_Create(
    SDL_Renderer* renderer, const char* str, const SDL_FRect destination, C4_UI_SymbolType symbol, float symbolWidth,
    float symbolHeight, int symbolRotationDegrees, const C4_UI_Theme* theme
) {
    C4_UI_Button* button = calloc(1, sizeof(C4_UI_Button));
    if (!button) {
        SDL_Log("Unable to allocate memory for button");
        return NULL;
    }
    if (!C4_UI_Button_InitProperties(button, renderer, str, destination, symbol, symbolWidth, symbolHeight, symbolRotationDegrees, theme)) {
        C4_UI_Button_Destroy(button);
        return NULL;
    }
    return button;
}

void C4_UI_Button_FreeResources(C4_UI_Button* button) {
    C4_UI_Text_FreeResources(&button->text);
}

void C4_UI_Button_Destroy(C4_UI_Button* button) {
    if (!button) {
        SDL_Log("Tried to destroy NULL button");
        return;
    }
    C4_UI_Button_FreeResources(button);
    free(button);
}

void C4_UI_Button_Draw(C4_UI_Button* button, SDL_Renderer* renderer) {
    if (!button) {
        SDL_Log("Button is NULL");
        return;
    }
    C4_UI_ButtonColorInfo* currentColors = &button->defaultColors;
    if (button->isPressed) {
        currentColors = &button->clickColors;
    } else if (button->isHovered) {
        currentColors = &button->hoverColors;
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

bool C4_UI_Button_HandleMouseEvents(C4_UI_Button* button, SDL_Event* event) {
    if (!button || !event) {
        SDL_Log("Button and/or event is NULL");
        return false;
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
            if (button->isHovered) {
                SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
                C4_PushEvent_SoundRequest(C4_SoundEffect_ButtonHover);
            } else {
                SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
            }
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event->button.button == SDL_BUTTON_LEFT && button->isHovered) {
            button->isPressed = true;
            C4_PushEvent_SoundRequest(C4_SoundEffect_ButtonClick);
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            bool wasClicked = button->isPressed && button->isHovered;
            button->isPressed = false;
            if (wasClicked)  {
                return true;
            }
        }
    }
    return false;
}

void C4_UI_Button_CenterInWindow(C4_UI_Button* button, C4_Axis axis) {
    if (!button) {
        SDL_Log("Button is NULL");
        return;
    }
    C4_UI_CenterInWindow(&button->background.destination, axis);
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