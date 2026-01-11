#include "Connect4/game/input/input.h"

static SDL_Gamepad* activeGamepad = NULL;

static C4_InputVerb C4_MapScancodeToVerb(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_W:
        case SDL_SCANCODE_UP: return C4_INPUT_NAV_UP;

        case SDL_SCANCODE_S:
        case SDL_SCANCODE_DOWN: return C4_INPUT_NAV_DOWN;

        case SDL_SCANCODE_D:
        case SDL_SCANCODE_RIGHT: return C4_INPUT_NAV_RIGHT;

        case SDL_SCANCODE_A:
        case SDL_SCANCODE_LEFT: return C4_INPUT_NAV_LEFT;

        case SDL_SCANCODE_KP_ENTER:
        case SDL_SCANCODE_RETURN: return C4_INPUT_CONFIRM;

        case SDL_SCANCODE_ESCAPE: return C4_INPUT_CANCEL;

        default: return C4_INPUT_NONE;
    }
}

static C4_InputVerb C4_MapButtonToVerb(Uint8 button) {
    switch (button) {
        case SDL_GAMEPAD_BUTTON_DPAD_UP: return C4_INPUT_NAV_UP;
        case SDL_GAMEPAD_BUTTON_DPAD_DOWN: return C4_INPUT_NAV_DOWN;
        case SDL_GAMEPAD_BUTTON_DPAD_LEFT: return C4_INPUT_NAV_LEFT;
        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT: return C4_INPUT_NAV_RIGHT;
        
        // South is A on Xbox, X on PlayStation, B on Switch
        case SDL_GAMEPAD_BUTTON_SOUTH: return C4_INPUT_CONFIRM;
        
        // East is B on Xbox, O on PlayStation, A on Switch
        case SDL_GAMEPAD_BUTTON_EAST: return C4_INPUT_CANCEL;
        
        case SDL_GAMEPAD_BUTTON_START: return C4_INPUT_CONFIRM;
        case SDL_GAMEPAD_BUTTON_BACK: return C4_INPUT_CANCEL;
        
        default: return C4_INPUT_NONE;
    }
}

void C4_Input_Shutdown(void) {
    if (activeGamepad) {
        SDL_CloseGamepad(activeGamepad);
        activeGamepad = NULL;
    }
}

C4_InputEvent C4_GetInput(SDL_Event* event) {
    C4_InputEvent input = {
        .verb = C4_INPUT_NONE,
        .state = C4_INPUT_STATE_RELEASED
    };
    switch (event->type) {
        case SDL_EVENT_KEY_DOWN: {
            if (event->key.repeat) {
                break;
            }
            input.verb = C4_MapScancodeToVerb(event->key.scancode);
            input.state = C4_INPUT_STATE_PRESSED;
        }; break;
        case SDL_EVENT_KEY_UP: {
            input.verb = C4_MapScancodeToVerb(event->key.scancode);
            input.state = C4_INPUT_STATE_RELEASED;
        }; break;
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
            input.verb = C4_MapButtonToVerb(event->gbutton.button);
            input.state = C4_INPUT_STATE_PRESSED;
        }; break;
        case SDL_EVENT_GAMEPAD_BUTTON_UP: {
            input.verb = C4_MapButtonToVerb(event->gbutton.button);
            input.state = C4_INPUT_STATE_RELEASED;
        }
        case SDL_EVENT_GAMEPAD_ADDED: {
            if (!activeGamepad) {
                activeGamepad = SDL_OpenGamepad(event->gdevice.which);
                SDL_Log("Gamepad Connected");
            }
        }; break;
        case SDL_EVENT_GAMEPAD_REMOVED: {
            if (activeGamepad && event->gdevice.which == SDL_GetGamepadID(activeGamepad)) {
                SDL_CloseGamepad(activeGamepad);
                activeGamepad = NULL;
                SDL_Log("Gamepad Disconnected");
            }
        }; break;
        default: break;
    }
    return input;
}