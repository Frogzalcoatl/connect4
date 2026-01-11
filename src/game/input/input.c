#include "Connect4/game/input/input.h"
#include "Connect4/ui/utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static SDL_Gamepad* connectedGamepads[C4_MAX_GAMEPADS] = {NULL};

static SDL_Gamepad* activeGamepad = NULL;

static bool onlyAcceptInputFromActiveGamepad = true;

static SDL_Scancode scancodeToInputVerb[C4_INPUT_VERB_COUNT] = {0};

void C4_Input_Init(void) {
    scancodeToInputVerb[C4_INPUT_VERB_CANCEL] = SDL_SCANCODE_ESCAPE;
}

void C4_Input_SetVerbScancode(C4_InputVerb inputVerb, SDL_Scancode scancode) {
    if (inputVerb <= 0 || inputVerb >= C4_INPUT_VERB_COUNT) {
        return;
    }
    scancodeToInputVerb[inputVerb] = scancode;
}

void C4_Gamepad_OnlyAcceptInputFromActiveGamepad(bool value) {
    onlyAcceptInputFromActiveGamepad = value;
}

static C4_InputVerb C4_MapScancodeToVerb(SDL_Scancode scancode) {
    for (C4_InputVerb inputVerb = 1; inputVerb < C4_INPUT_VERB_COUNT; inputVerb++) {
        if (scancode == scancodeToInputVerb[inputVerb]) {
            return inputVerb;
        }
    } 
    return C4_INPUT_VERB_NONE;
}

static C4_InputVerb C4_MapButtonToVerb(Uint8 button) {
    switch (button) {
        case SDL_GAMEPAD_BUTTON_DPAD_UP: return C4_INPUT_VERB_NAV_UP;
        case SDL_GAMEPAD_BUTTON_DPAD_DOWN: return C4_INPUT_VERB_NAV_DOWN;
        case SDL_GAMEPAD_BUTTON_DPAD_LEFT: return C4_INPUT_VERB_NAV_LEFT;
        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT: return C4_INPUT_VERB_NAV_RIGHT;
        
        // South is A on Xbox, X on PlayStation, B on Switch
        case SDL_GAMEPAD_BUTTON_SOUTH: return C4_INPUT_VERB_CONFIRM;
        
        // East is B on Xbox, O on PlayStation, A on Switch
        case SDL_GAMEPAD_BUTTON_EAST: return C4_INPUT_VERB_CANCEL;
        
        case SDL_GAMEPAD_BUTTON_START: return C4_INPUT_VERB_CONFIRM;
        case SDL_GAMEPAD_BUTTON_BACK: return C4_INPUT_VERB_CANCEL;
        
        default: return C4_INPUT_VERB_NONE;
    }
}

static void C4_RefreshActiveGamepad(void) {
    if (activeGamepad != NULL) {
        return;
    }
    for (int i = 0; i < C4_MAX_GAMEPADS; i++) {
        if (connectedGamepads[i] != NULL) {
            activeGamepad = connectedGamepads[i];
            SDL_Log("Active Gamepad automatically set to index %d", i);
            return;
        }
    }
}

void C4_Gamepad_SetActiveIndex(int index) {
    if (index >= 0 && index < C4_MAX_GAMEPADS && connectedGamepads[index] != NULL) {
        activeGamepad = connectedGamepads[index];
        SDL_Log("Active Gamepad manually set to index %d", index);
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
        .verb = C4_INPUT_VERB_NONE,
        .state = C4_INPUT_STATE_RELEASED
    };

    switch (event->type) {
        case SDL_EVENT_GAMEPAD_ADDED: {
            int slot = -1;
            for (int i = 0; i < C4_MAX_GAMEPADS; i++) {
                if (connectedGamepads[i] == NULL) {
                    slot = i;
                    break;
                }
            }
            if (slot == -1) {
                SDL_Log("Gamepad connected but ignored (Max limit reached)");
                return input;
            }
            SDL_Gamepad* newPad = SDL_OpenGamepad(event->gdevice.which);
            if (newPad) {
                connectedGamepads[slot] = newPad;
                SDL_Log("Gamepad Added at index %d", slot);
                C4_RefreshActiveGamepad();
            }
        }; return input;
        case SDL_EVENT_GAMEPAD_REMOVED: {
            for (int i = 0; i < C4_MAX_GAMEPADS; i++) {
                if (
                    connectedGamepads[i] && 
                    SDL_GetGamepadID(connectedGamepads[i]) == event->gdevice.which
                ) {
                    if (connectedGamepads[i] == activeGamepad) {
                        activeGamepad = NULL;
                        SDL_Log("Gamepad Disconnected at index %d", i);
                    }
                    SDL_CloseGamepad(connectedGamepads[i]);
                    connectedGamepads[i] = NULL;
                    C4_RefreshActiveGamepad();
                    return input;
                }
            }
        }; return input;
        case SDL_EVENT_KEY_DOWN: {
            if (event->key.repeat) {
                return input;
            }
            input.verb = C4_MapScancodeToVerb(event->key.scancode);
            input.state = C4_INPUT_STATE_PRESSED;
        }; return input;
        case SDL_EVENT_KEY_UP: {
            input.verb = C4_MapScancodeToVerb(event->key.scancode);
            input.state = C4_INPUT_STATE_RELEASED;
        }; return input;
    }

    if (
        onlyAcceptInputFromActiveGamepad &&
        event->type >= SDL_EVENT_GAMEPAD_AXIS_MOTION &&
        event->type <= SDL_EVENT_GAMEPAD_STEAM_HANDLE_UPDATED &&
        event->gdevice.which != SDL_GetGamepadID(activeGamepad)
    ) {
        return input;
    }
    switch (event->type) {
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN: {
            input.verb = C4_MapButtonToVerb(event->gbutton.button);
            input.state = C4_INPUT_STATE_PRESSED;
        }; break;
        case SDL_EVENT_GAMEPAD_BUTTON_UP: {
            input.verb = C4_MapButtonToVerb(event->gbutton.button);
            input.state = C4_INPUT_STATE_RELEASED;
        }; break;
        default: break;
    }
    return input;
}

void C4_Gamepad_GetNames(char** returnValue, size_t returnValueSize) {
    if (!returnValue) {
        return;
    }
    size_t limit = C4_ULLMin(C4_MAX_GAMEPADS, returnValueSize);

    for (size_t i = 0; i < limit; i++) {
        SDL_Gamepad* currentPad = connectedGamepads[i];

        if (currentPad == NULL) {
            returnValue[i] = NULL;
            continue;
        }

        const char* rawName = SDL_GetGamepadName(currentPad);
        if (!rawName) rawName = "Unknown Controller";

        int totalMatches = 0;
        int myInstanceIndex = 1;

        for (size_t j = 0; j < limit; j++) {
            if (connectedGamepads[j] != NULL) {
                const char* otherName = SDL_GetGamepadName(connectedGamepads[j]);
                if (otherName && strcmp(rawName, otherName) == 0) {
                    totalMatches++;
                    if (j < i) {
                        myInstanceIndex++;
                    }
                }
            }
        }
        
        if (totalMatches > 1) {
            size_t neededLen = strlen(rawName) + 16; 
            returnValue[i] = malloc(neededLen);
            if (returnValue[i]) {
                snprintf(returnValue[i], neededLen, "%s (%d)", rawName, myInstanceIndex);
            }
        } else {
            size_t len = strlen(rawName);
            returnValue[i] = malloc(len + 1);
            if (returnValue[i]) {
                strcpy(returnValue[i], rawName);
            }
        }
    }
}

void C4_Gamepad_GetActiveName(char* returnValue, size_t returnValueSize) {
    if (!returnValue || returnValueSize == 0) {
        return;
    }

    if (!activeGamepad) {
        snprintf(returnValue, returnValueSize, "None");
        return;
    }

    int activeIndex = -1;
    for (int i = 0; i < C4_MAX_GAMEPADS; i++) {
        if (connectedGamepads[i] == activeGamepad) {
            activeIndex = i;
            break;
        }
    }

    if (activeIndex == -1) {
        snprintf(returnValue, returnValueSize, "Unknown");
        return;
    }

    const char* rawName = SDL_GetGamepadName(activeGamepad);
    if (!rawName) rawName = "Unknown Controller";

    int totalMatches = 0;
    int myInstanceIndex = 1;

    for (int j = 0; j < C4_MAX_GAMEPADS; j++) {
        if (connectedGamepads[j] != NULL) {
            const char* otherName = SDL_GetGamepadName(connectedGamepads[j]);
            
            if (otherName && strcmp(rawName, otherName) == 0) {
                totalMatches++;
                if (j < activeIndex) {
                    myInstanceIndex++;
                }
            }
        }
    }

    if (totalMatches > 1) {
        snprintf(returnValue, returnValueSize, "%s (%d)", rawName, myInstanceIndex);
    } else {
        snprintf(returnValue, returnValueSize, "%s", rawName);
    }
}