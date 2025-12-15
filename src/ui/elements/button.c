#include "Connect4/ui/elements/button.h"
#include "Connect4/ui/cursorManager.h"
#include "Connect4/assets/sounds.h"
#include "Connect4/constants.h"
#include <stdlib.h>

C4_UI_Button* C4_UI_Button_Create(SDL_Renderer* renderer, const char* str, C4_FontType font, float ptSize, const SDL_FRect background) {
    if (!str || !renderer) {
        SDL_Log("Unable to create Button. str and/or renderer is NULL");
        return NULL;
    }
    C4_UI_Button* button = calloc(1, sizeof(C4_UI_Button));
    if (!button) {
        SDL_Log("Unable to allocate memory for button");
        return NULL;
    }
    button->defaultColors = (C4_UI_ButtonColorInfo){C4_BUTTON_DEFAULTCOLOR_BACKGROUND, C4_BUTTON_DEFAULTCOLOR_TEXT};
    button->hoverColors = (C4_UI_ButtonColorInfo){C4_BUTTON_HOVERCOLOR_BACKGROUND, C4_BUTTON_HOVERCOLOR_TEXT};
    button->clickColors = (C4_UI_ButtonColorInfo){C4_BUTTON_CLICKCOLOR_BACKGROUND, C4_BUTTON_CLICKCOLOR_TEXT};
    button->background = C4_UI_Rectangle_Create(background, button->defaultColors.background);
    if (!button->background) {
        SDL_Log("Unable to create button background");
        return NULL;
    }
    button->text = C4_UI_Text_Create(renderer, str, font, ptSize, background.x, background.y, 0);
    if (!button->text) {
        SDL_Log("Unable to create button text");
        return NULL;
    }
    button->isHovered = false;
    button->isPressed = false;
    C4_UI_Button_CenterTextInBackground(button, C4_Axis_XY);
    return button;
}

void C4_UI_Button_Destroy(C4_UI_Button* button) {
    if (!button) {
        SDL_Log("Tried to destroy NULL button");
        return;
    }
    C4_UI_Text_Destroy(button->text);
    C4_UI_Rectangle_Destroy(button->background);
    free(button);
}

void C4_UI_Button_Draw(C4_UI_Button* button, SDL_Renderer* renderer) {
    if (!button) {
        SDL_Log("Button is NULL");
        return;
    }
    C4_UI_Rectangle_Draw(button->background, renderer);
    C4_UI_Text_Draw(button->text, renderer);
}

bool C4_UI_Button_HandleMouseEvents(C4_UI_Button* button, SDL_Event* event, SDL_Renderer* renderer) {
    if (!button || !event) {
        SDL_Log("Button and/or event is NULL");
        return false;
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
                SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
                C4_PlaySound(C4_SoundEffect_ButtonHover);
            } else {
                button->background->color = button->defaultColors.background;
                button->text->color = button->defaultColors.text;
                SDL_SetCursor(C4_GetSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
            }
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event->button.button == SDL_BUTTON_LEFT && button->isHovered) {
            button->isPressed = true;
            button->background->color = button->clickColors.background;
            button->text->color = button->clickColors.text;
            C4_PlaySound(C4_SoundEffect_ButtonClick);
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (event->button.button == SDL_BUTTON_LEFT) {
            if (button->isPressed && button->isHovered)  {
                button->background->color = button->hoverColors.background;
                button->text->color = button->hoverColors.text;
                return true;
            }
            button->isPressed = false;
        }
    }
    return false;
}

void C4_UI_Button_CenterTextInBackground(C4_UI_Button* button, C4_Axis axis) {
    if (!button) {
        SDL_Log("Button is NULL");
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

void C4_UI_Button_CenterInWindow(C4_UI_Button* button, C4_Axis axis) {
    if (!button) {
        SDL_Log("Button is NULL");
        return;
    }
    C4_UI_Rectangle_CenterInWindow(button->background, axis);
    C4_UI_Button_CenterTextInBackground(button, axis);
}

void C4_UI_Button_TransformResize(C4_UI_Button* button, float x, float y, float w, float h) {
    if (!button) {
        SDL_Log("Tried to resize NULL button");
        return;
    }
    button->background->rectangle = (SDL_FRect){x, y, w, h};
    C4_UI_Button_CenterTextInBackground(button, C4_Axis_XY);
}