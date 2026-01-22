#include "Connect4/input/gamepad.h"
#include "Connect4/ui/utils.h"
#include "Connect4/game/consoleOutput.h"
#include "Connect4/tools/dynamicArray.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct C4_GamepadList {
    SDL_Gamepad** data;
    size_t count;
    size_t capacity;
} C4_GamepadList;
C4_GamepadList connectedGamepads;

static SDL_Gamepad* activeGamepad = NULL;

static bool onlyAcceptInputFromActiveGamepad = true;

static C4_VerbMapping verbMappings[C4_INPUT_VERB_COUNT] = {0};

typedef struct {
    bool isDown;
    bool isRepeating;
    float timer;
} C4_InputVerb_State;
static C4_InputVerb_State verbStates[C4_INPUT_VERB_COUNT] = {0};

static const float INPUT_REPEAT_DELAY = 0.5f;
static const float INPUT_REPEAT_INTERVAL = 0.1f;

void C4_Input_ConnectScancodeToVerb(C4_InputVerb inputVerb, SDL_Scancode scancode) {
    assert(inputVerb > 0 && inputVerb < C4_INPUT_VERB_COUNT);
    assert(scancode > SDL_SCANCODE_UNKNOWN && scancode < SDL_SCANCODE_COUNT);

    C4_VerbMapping* map = &verbMappings[inputVerb];
    for (size_t i = 0; i < map->count; i++) {
        if (map->data[i] == scancode) {
            return;
        }
    }
    
    C4_DynamicArray_Push_Back(*map, scancode);
}

void C4_Input_DisconnectScancodeFromVerb(C4_InputVerb inputVerb, SDL_Scancode scancode) {
    assert(inputVerb > 0 && inputVerb < C4_INPUT_VERB_COUNT);
    assert(scancode > SDL_SCANCODE_UNKNOWN && scancode < SDL_SCANCODE_COUNT);

    C4_VerbMapping* map = &verbMappings[inputVerb];

    for (size_t i = 0; i < map->count; i++) {
        if (map->data[i] == scancode) {
            C4_DynamicArray_Remove_ReplaceWithBack(*map, i);
            return;
        }
    }
}

void C4_Input_Init(void) {
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_CANCEL, SDL_SCANCODE_ESCAPE);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_UP, SDL_SCANCODE_UP);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_DOWN, SDL_SCANCODE_DOWN);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_RIGHT, SDL_SCANCODE_RIGHT);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_NAV_LEFT, SDL_SCANCODE_LEFT);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_CONFIRM, SDL_SCANCODE_RETURN);
    C4_Input_ConnectScancodeToVerb(C4_INPUT_VERB_CONFIRM, SDL_SCANCODE_KP_ENTER);
    C4_Log("Initialized C4_Input");
}

void C4_Gamepad_OnlyAcceptInputFromActiveGamepad(bool value) {
    onlyAcceptInputFromActiveGamepad = value;
}

static C4_InputVerb C4_MapScancodeToVerb(SDL_Scancode scancode) {
    for (size_t i = 1; i < C4_INPUT_VERB_COUNT; i++) {
        C4_VerbMapping* map = &verbMappings[i];
        for (size_t j = 0; j < map->count; j++) {
            if (map->data[j] == scancode) {
                return (C4_InputVerb)i;
            }
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
    for (size_t i = 0; i < connectedGamepads.count; i++) {
        if (connectedGamepads.data[i]) {
            C4_Gamepad_SetActiveIndex(i);
            return;
        }
    }
}

void C4_Gamepad_SetActiveIndex(size_t index) {
    assert(index < connectedGamepads.count);

    if (!connectedGamepads.data[index]) {
        C4_Warn(
            SDL_LOG_CATEGORY_INPUT,
            "Unable to set active gampad to index %zu",
            index
        );
        return;
    }
    activeGamepad = connectedGamepads.data[index];
    char activeGamepadName[256];
    C4_Gamepad_GetActiveName(activeGamepadName, sizeof(activeGamepadName));
    C4_Log("Active gamepad set to index %zu: %s", index, activeGamepadName);
}

void C4_Input_Shutdown(void) {
    activeGamepad = NULL;

    for(size_t i = 0; i < connectedGamepads.count; i++) {
        if(connectedGamepads.data[i]) {
            SDL_CloseGamepad(connectedGamepads.data[i]);
        }
    }
    C4_DynamicArray_Free(connectedGamepads);

    for (int i = 0; i < C4_INPUT_VERB_COUNT; i++) {
        C4_DynamicArray_Free(verbMappings[i]);
    }
    C4_Log("Shutdown C4_Input");
}

void C4_Input_ResetVerbState(C4_InputVerb verb) {
    assert(verb > 0 && verb < C4_INPUT_VERB_COUNT);

    verbStates[verb].isDown = false;
    verbStates[verb].isRepeating = false;
    verbStates[verb].timer = 0.f;
}

C4_InputEvent C4_GetInput(SDL_Event* event) {
    assert(event);

    C4_InputEvent input = {
        .verb = C4_INPUT_VERB_NONE,
        .state = C4_INPUT_STATE_RELEASED
    };

    switch (event->type) {
        case SDL_EVENT_GAMEPAD_ADDED: {
            SDL_Gamepad* newPad = SDL_OpenGamepad(event->gdevice.which);
            if (!newPad) {
                return input;
            }

            C4_DynamicArray_Push_Ptr_FillHole(connectedGamepads, newPad);

            int openIndex = -1;
            for(size_t i = 0; i < connectedGamepads.count; i++) {
                if(connectedGamepads.data[i] == newPad) {
                    openIndex = (int)i;
                    break;
                }
            }
            
            char gamepadName[256];
            if (activeGamepad) {
                C4_Gamepad_GetNameAtIndex(openIndex, gamepadName, sizeof(gamepadName));
            } else {
                snprintf(gamepadName, sizeof(gamepadName), "%s", SDL_GetGamepadName(newPad));
            }
            C4_Log(
                "Gamepad connected at index %d: %s",
                openIndex, gamepadName
            );
            C4_RefreshActiveGamepad();
        }; return input;
        case SDL_EVENT_GAMEPAD_REMOVED: {
            for (size_t i = 0; i < connectedGamepads.count; i++) {
                if (
                    connectedGamepads.data[i] && 
                    SDL_GetGamepadID(connectedGamepads.data[i]) == event->gdevice.which
                ) {
                    if (connectedGamepads.data[i] == activeGamepad) {
                        activeGamepad = NULL;
                    }
                    SDL_CloseGamepad(connectedGamepads.data[i]);
                    
                    C4_DynamicArray_Remove_Ptr_MakeHole(connectedGamepads, i);

                    C4_Log("Disconnected gamepad at index %zu", i);
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
        }; break;
        case SDL_EVENT_KEY_UP: {
            input.verb = C4_MapScancodeToVerb(event->key.scancode);
            input.state = C4_INPUT_STATE_RELEASED;
        }; break;
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
    if (input.verb == C4_INPUT_VERB_NONE) {
        return input;
    }
    if (input.state == C4_INPUT_STATE_PRESSED) {
        if (!verbStates[input.verb].isDown) {
            verbStates[input.verb].isDown = true;
            verbStates[input.verb].isRepeating = false;
            verbStates[input.verb].timer = 0.f;
        }
    } else {
        C4_Input_ResetVerbState(input.verb);
    }
    return input;
}

bool C4_Input_CheckRepeat(float deltaTime, C4_InputEvent* outEvent) {
    for (int i = 1; i < C4_INPUT_VERB_COUNT; i++) {
        if (!verbStates[i].isDown) {
            continue;
        }

        verbStates[i].timer += deltaTime;

        float threshold = verbStates[i].isRepeating ? INPUT_REPEAT_INTERVAL : INPUT_REPEAT_DELAY;

        if (verbStates[i].timer >= threshold) {
            verbStates[i].timer -= threshold;
            verbStates[i].isRepeating = true;
            if (outEvent) {
                outEvent->verb = (C4_InputVerb)i;
                outEvent->state = C4_INPUT_STATE_PRESSED;
            }
            return true; 
        }
    }
    return false;
}

void C4_Gamepad_GetNames(char** returnValue, size_t returnValueSize) {
    if (!returnValue || returnValueSize == 0) {
        return;
    }

    for (size_t i = 0; i < returnValueSize; i++) {
        returnValue[i] = NULL;
    }

    size_t foundCount = 0;

    for (size_t i = 0; i < connectedGamepads.count; i++) {
        if (foundCount >= returnValueSize) {
            break;
        }

        SDL_Gamepad* currentPad = connectedGamepads.data[i];

        if (currentPad == NULL) {
            continue;
        }

        const char* rawName = SDL_GetGamepadName(currentPad);
        if (!rawName) {
            rawName = "Unknown Controller";
        }

        int totalMatches = 0;
        int myInstanceIndex = 1;

        for (size_t j = 0; j < connectedGamepads.count; j++) {
            if (connectedGamepads.data[j]) {
                const char* otherName = SDL_GetGamepadName(connectedGamepads.data[j]);
                if (otherName && strcmp(rawName, otherName) == 0) {
                    totalMatches++;
                    if (j < i) {
                        myInstanceIndex++;
                    }
                }
            }
        }
        
        size_t destIndex = foundCount;

        if (totalMatches > 1) {
            size_t neededLen = strlen(rawName) + 16; 
            returnValue[destIndex] = SDL_malloc(neededLen);
            if (returnValue[destIndex]) {
                snprintf(returnValue[destIndex], neededLen, "%s (%d)", rawName, myInstanceIndex);
            }
        } else {
            size_t len = strlen(rawName);
            returnValue[destIndex] = SDL_malloc(len + 1);
            if (returnValue[destIndex]) {
                strcpy(returnValue[destIndex], rawName);
            }
        }
        
        foundCount++;
    }
}

void C4_Gamepad_GetNameAtIndex(size_t index, char* returnValue, size_t returnValueSize) {
    assert(index < connectedGamepads.count);
    if (!returnValue || returnValueSize == 0) {
        return;
    }

    if (!connectedGamepads.data[index]) {
        snprintf(returnValue, returnValueSize, "None");
        return;
    }

    const char* rawName = SDL_GetGamepadName(connectedGamepads.data[index]);
    if (!rawName) {
        rawName = "Unknown Controller";
    }

    int totalMatches = 0;
    int myInstanceIndex = 1;

    for (size_t j = 0; j < connectedGamepads.count; j++) {
        if (connectedGamepads.data[j] != NULL) {
            const char* otherName = SDL_GetGamepadName(connectedGamepads.data[j]);
            
            if (otherName && strcmp(rawName, otherName) == 0) {
                totalMatches++;
                if (j < index) {
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

void C4_Gamepad_GetActiveName(char* returnValue, size_t returnValueSize) {
    assert(returnValue && returnValueSize > 0);

    if (!activeGamepad) {
        snprintf(returnValue, returnValueSize, "None");
        return;
    }

    int64_t activeIndex = -1;
    for (size_t i = 0; i < connectedGamepads.count; i++) {
        if (connectedGamepads.data[i] == activeGamepad) {
            activeIndex = i;
            break;
        }
    }

    if (activeIndex == -1) {
        snprintf(returnValue, returnValueSize, "Unknown");
        return;
    }

    C4_Gamepad_GetNameAtIndex(activeIndex, returnValue, returnValueSize);    
}