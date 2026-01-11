#pragma once
#include "Connect4/game/input/input.h"

typedef struct {
    float delay;
    float interval;
    float pressTimer;
} C4_UI_Interaction_WhilePressedTiming;

typedef struct {
    bool isHovered;
    bool isPressed;
    bool isFocusable;
    bool isActive;
    bool isRepeating;

    void (*OnHover)(void* context);
    void (*WhilePressed)(void* context);
    void (*OnPress)(void* context);
    void (*OnRelease)(void* context);
    void* context;

    C4_UI_Interaction_WhilePressedTiming timing;
} C4_UI_Interaction;

void C4_UI_Interaction_Update(C4_UI_Interaction* input, float deltaTime);
bool C4_UI_Interaction_HandleAction(C4_UI_Interaction* input, C4_InputEvent event);
bool C4_UI_Interaction_HandleMouseEvents(SDL_FRect absoluteRect, C4_UI_Interaction* input, SDL_Event* event);
void C4_UI_Interaction_Reset(C4_UI_Interaction* input);