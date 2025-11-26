#include "Connect4/ui/button.h"
#include "Connect4/ui/cursorManager.h"
#include <stdlib.h>

C4_Button* C4_Button_Create(
    SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, const SDL_FRect background, const C4_ButtonColorInfo defaultColors,
    const C4_ButtonColorInfo hoverColors, const C4_ButtonColorInfo clickColors, C4_ScreenChangeRequest onClick
) {
    if (!str || !renderer) {
        return NULL;
    }
    C4_Button* button = calloc(1, sizeof(C4_Button));
    if (!button) {
        return NULL;
    }
    button->defaultColors = defaultColors;
    button->hoverColors = hoverColors;
    button->clickColors = clickColors;
    button->background = C4_RectangleUIElement_Create(background, button->defaultColors.background);
    if (!button->background) {
        return NULL;
    }
    button->text = C4_TextUIElement_Create(renderer, str, font, ptSize, button->defaultColors.text, background.x, background.y);
    if (!button->text) {
        return NULL;
    }
    button->isHovered = false;
    button->isPressed = false;
    button->onClick = onClick;
    C4_Button_CenterTextInBackground(button, C4_Axis_XY);
    return button;
}

void C4_Button_Destroy(C4_Button* button) {
    if (!button) {
        return;
    }
    C4_TextUIElement_Destroy(button->text);
    C4_RectangleUIElement_Destroy(button->background);
    free(button);
}

void C4_Button_Draw(C4_Button* button, SDL_Renderer* renderer) {
    if (!button) {
        return;
    }
    C4_RectangleUIElement_Draw(button->background, renderer);
    C4_TextUIElement_Draw(button->text, renderer);
}

C4_ScreenChangeRequest C4_Button_HandleMouseEvents(C4_Button* button, SDL_Event* event, SDL_Renderer* renderer) {
    if (!button || !event) {
        return C4_ScreenChangeRequest_None;
    }
    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        bool currentlyHovered = (
            event->motion.x >= button->background->rectangle.x &&
            event->motion.x <= button->background->rectangle.x + button->background->rectangle.w &&
            event->motion.y >= button->background->rectangle.y &&
            event->motion.y <= button->background->rectangle.y + button->background->rectangle.h
        );
        if (currentlyHovered != button->isHovered) {
            button->isHovered = currentlyHovered;
            if (button->isHovered) {
                button->background->color = button->hoverColors.background;
                button->text->color = button->hoverColors.text;
                C4_TextUIElement_Refresh(button->text, renderer);
                SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
            } else {
                button->background->color = button->defaultColors.background;
                button->text->color = button->defaultColors.text;
                C4_TextUIElement_Refresh(button->text, renderer);
                SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
            }
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event->button.button == SDL_BUTTON_LEFT && button->isHovered) {
            button->isPressed = true;
            button->background->color = button->clickColors.background;
            button->text->color = button->clickColors.text;
            C4_TextUIElement_Refresh(button->text, renderer);
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            if (button->isPressed && button->isHovered)  {
                button->background->color = button->hoverColors.background;
                button->text->color = button->hoverColors.text;
                C4_TextUIElement_Refresh(button->text, renderer);
                return button->onClick;
            }
            button->isPressed = false;
        }
    }
    return C4_ScreenChangeRequest_None;
}

void C4_Button_CenterTextInBackground(C4_Button* button, C4_Axis axis) {
    if (!button) {
        return;
    }
    if (axis == C4_Axis_X || axis == C4_Axis_XY) {
        float backgroundCenterX = button->background->rectangle.x + (button->background->rectangle.w / 2.f);
        button->text->destination.x = backgroundCenterX - (button->text->destination.w / 2.0f);
    }
    if (axis == C4_Axis_Y || axis == C4_Axis_XY) {
        float bcakgroundCenterY = button->background->rectangle.y + (button->background->rectangle.h / 2.f);
        button->text->destination.y = bcakgroundCenterY - (button->text->destination.h / 2.f);
    }
}

void C4_Button_CenterInWindow(C4_Button* button, C4_Axis axis) {
    if (!button) {
        return;
    }
    C4_RectangleUIElement_CenterInWindow(button->background, axis);
    C4_Button_CenterTextInBackground(button, axis);
}