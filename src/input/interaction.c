#include "Connect4/input/interaction.h"
#include "Connect4/physics/intersection.h"
#include "Connect4/game/events.h"
#include "Connect4/game/consoleOutput.h"
#include <assert.h>

void C4_UI_Interaction_Update(C4_UI_Interaction* input, float deltaTime) {
    if (!input) {
        return;
    }

    assert(deltaTime >= 0.0f);
    
    if (!input->isActive) {
        input->isPressed = false;
        input->isHovered = false;
        input->isRepeating = false;
        input->timing.pressTimer = 0.f;
    }
    if (!input->isPressed || !input->WhilePressed) {
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
            if (input->sounds.whilePressed != C4_SoundEffect_None) {
                C4_PushEvent_PlaySound(input->sounds.whilePressed);
            }
            input->timing.pressTimer = 0.f;
            input->isRepeating = true;
        }
    } else {
        if (input->timing.pressTimer >= input->timing.interval) {
            if (input->WhilePressed) {
                input->WhilePressed(input->context);
            }
            if (input->sounds.whilePressed != C4_SoundEffect_None) {
                C4_PushEvent_PlaySound(input->sounds.whilePressed);
            }
            input->timing.pressTimer = 0.f;
        }
    }
}

bool C4_UI_Interaction_HandleAction(C4_UI_Interaction* input, C4_InputEvent event) {
    if (!input) {
        return false;
    }

    if (event.verb == C4_INPUT_VERB_CANCEL) {
        if (event.state == C4_INPUT_STATE_PRESSED && input->OnCancel) {
            input->OnCancel(input->context);
            if (input->sounds.onCancel != C4_SoundEffect_None) {
                C4_PushEvent_PlaySound(input->sounds.onCancel);
            }
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
        if (input->OnPress) {
            input->OnPress(input->context);
        }
        if (input->sounds.onPress != C4_SoundEffect_None) {
            C4_PushEvent_PlaySound(input->sounds.onPress);
        }
        return true;
    } else if (event.state == C4_INPUT_STATE_RELEASED) {
        if (input->isPressed) {
            input->isPressed = false;
            if (input->OnRelease) {
                input->OnRelease(input->context);
            }
            if (input->sounds.onRelease != C4_SoundEffect_None) {
                C4_PushEvent_PlaySound(input->sounds.onRelease);
            }
        }
        return true;
    }
    return false;
}

bool C4_UI_Interaction_HandleMouseEvents(
    C4_UI_Interaction* input,
    SDL_Event* event,
    C4_UI_ShapeType shape,
    SDL_FRect absoluteRect,
    float rotationDegrees,
    C4_UI_Mirror mirror
) {
    if (!input || !event) {
        return false;
    }
    
    assert(shape >= 0 && shape < C4_UI_ShapeType_Count);
    assert(mirror >= C4_UI_Mirror_None && mirror < C4_UI_Mirror_Count);

    if (!input->isActive) {
        return false;
    }
    
    SDL_FRect* rect = &absoluteRect;
    SDL_MouseMotionEvent* motion = &event->motion;
    Uint8 mouseButton = event->button.button;

    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        bool currentlyHovered = C4_IsPointInsideShape(shape, (SDL_FPoint){motion->x, motion->y}, *rect, rotationDegrees, mirror);
        if (currentlyHovered != input->isHovered) {
            input->isHovered = currentlyHovered;
            C4_PushEvent_SetCursor(currentlyHovered ? SDL_SYSTEM_CURSOR_POINTER : SDL_SYSTEM_CURSOR_DEFAULT);
            if (input->isHovered) {
                if (input->OnHover) {
                    input->OnHover(input->context);
                }
                bool isTouch = (event->motion.which == SDL_TOUCH_MOUSEID);
                if (!isTouch && input->sounds.onHover != C4_SoundEffect_None) {
                    C4_PushEvent_PlaySound(input->sounds.onHover);
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
            if (input->sounds.onPress != C4_SoundEffect_None) {
                C4_PushEvent_PlaySound(input->sounds.onPress);
            }
            if (input->WhilePressed) {
                input->WhilePressed(input->context);
            }
            if (input->sounds.whilePressed != C4_SoundEffect_None) {
                C4_PushEvent_PlaySound(input->sounds.whilePressed);
            }
            return true;
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        if (mouseButton == SDL_BUTTON_LEFT) {
            bool wasReleased = input->isPressed && input->isHovered;
            input->isPressed = false;
            bool isTouch = (event->motion.which == SDL_TOUCH_MOUSEID);
            if (isTouch) {
                input->isHovered = false;
            }
            if (wasReleased) {
                if (input->OnRelease) {
                    input->OnRelease(input->context);
                }
                if (input->sounds.onRelease != C4_SoundEffect_None) {
                    C4_PushEvent_PlaySound(input->sounds.onRelease);
                }
                return true;
            }
        }
    }
    return false;
}

void C4_UI_Interaction_Reset(C4_UI_Interaction* input) {
    assert(input);

    input->isHovered = false;
    input->isPressed = false;
    input->isRepeating = false;
}