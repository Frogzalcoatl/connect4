#include "Connect4/ui/interaction.h"
#include "Connect4/ui/cursorManager.h"

void C4_UI_Interaction_Update(C4_UI_Interaction* input, float deltaTime) {
    if (!input) {
        return;
    }
    if (!input->isActive) {
        input->isPressed = false;
        input->isHovered = false;
        input->isRepeating = false;
        input->timing.pressTimer = 0.f;
    }
    if (!input || !input->isPressed || !input->WhilePressed) {
        input->timing.pressTimer = 0.f;
        input->isRepeating = false;
        return;
    }
    input->timing.pressTimer += deltaTime;
    if (!input->isRepeating) {
        if (input->timing.pressTimer >= input->timing.delay) {
            if (input->WhilePressed) {
                input->WhilePressed(input->context);
            }
            input->timing.pressTimer = 0.f;
            input->isRepeating = true;
        }
    } else {
        if (input->timing.pressTimer >= input->timing.interval) {
            if (input->WhilePressed) {
                input->WhilePressed(input->context);
            }
            input->timing.pressTimer = 0.f;
        }
    }
}

bool C4_UI_Interaction_HandleAction(C4_UI_Interaction* input, C4_InputEvent event) {
    if (event.verb == C4_INPUT_VERB_CANCEL) {
        if (event.state == C4_INPUT_STATE_PRESSED && input->OnCancel) {
            input->OnCancel(input->context);
            return true;
        }
        return false;
    }
    if (!input->isActive || !input->isFocusable) {
        return false;
    }
    if (event.verb != C4_INPUT_VERB_CONFIRM) {
        return false;
    }
    if (event.state == C4_INPUT_STATE_PRESSED) {
        input->isPressed = true;
        if (input->OnPress) input->OnPress(input->context);
        return true;
    } else if (event.state == C4_INPUT_STATE_RELEASED) {
        if (input->isPressed) {
            input->isPressed = false;
            if (input->OnRelease) input->OnRelease(input->context);
        }
        return true;
    }
    return false;
}

bool C4_UI_Interaction_HandleMouseEvents(SDL_FRect absoluteRect, C4_UI_Interaction* input, SDL_Event* event) {
    if (!input->isActive) {
        return false;
    }
    
    SDL_FRect* rect = &absoluteRect;
    SDL_MouseMotionEvent* motion = &event->motion;
    Uint8 mouseButton = event->button.button;

    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        bool currentlyHovered = (
            motion->x >= rect->x &&
            motion->x <= rect->x + rect->w &&
            motion->y >= rect->y &&
            motion->y <= rect->y + rect->h
        );
        if (currentlyHovered != input->isHovered) {
            input->isHovered = currentlyHovered;
            SDL_SetCursor(C4_GetSystemCursor(currentlyHovered ? SDL_SYSTEM_CURSOR_POINTER : SDL_SYSTEM_CURSOR_DEFAULT));
            if (input->isHovered) {
                if (input->OnHover) {
                    input->OnHover(input->context);
                }
                return true;
            }
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (mouseButton == SDL_BUTTON_LEFT && input->isHovered) {
            input->isPressed = true;
            if (input->OnPress) {
                input->OnPress(input->context);
            }
            if (input->WhilePressed) {
                input->WhilePressed(input->context);
            }
            return true;
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP || event->type == SDL_EVENT_FINGER_UP) {
        if (mouseButton == SDL_BUTTON_LEFT) {
            bool wasReleased = input->isPressed && input->isHovered;
            input->isPressed = false;
            if (event->type == SDL_EVENT_FINGER_UP) {
                input->isHovered = false;
            }
            if (wasReleased) {
                if (input->OnRelease) {
                    input->OnRelease(input->context);
                }
                return true;
            }
        }
    }
    return false;
}

void C4_UI_Interaction_Reset(C4_UI_Interaction* input) {
    if (!input) {
        return;
    }
    input->isHovered = false;
    input->isPressed = false;
    input->isRepeating = false;
}