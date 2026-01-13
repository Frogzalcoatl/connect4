#pragma once
#include "Connect4/input/gamepad.h"
#include "Connect4/assets/sounds.h"
#include "Connect4/ui/types.h"

typedef struct {
    float delay;
    float interval;
    float pressTimer;
} C4_UI_Interaction_WhilePressedTiming;

typedef struct {
    C4_SoundEffect onHover;
    C4_SoundEffect whilePressed;
    C4_SoundEffect onPress;
    C4_SoundEffect onRelease;
    C4_SoundEffect onCancel;
} C4_UI_Interaction_Sounds;

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
    void (*OnCancel)(void* context);
    void* context;

    C4_UI_Interaction_Sounds sounds;

    C4_UI_Interaction_WhilePressedTiming timing;
} C4_UI_Interaction;

void C4_UI_Interaction_Update(C4_UI_Interaction* input, float deltaTime);
bool C4_UI_Interaction_HandleAction(C4_UI_Interaction* input, C4_InputEvent event);
bool C4_UI_Interaction_HandleMouseEvents(
    C4_UI_Interaction* input,
    SDL_Event* event,
    C4_UI_ShapeType shape,
    SDL_FRect absoluteRect,
    float rotationDegrees
);
void C4_UI_Interaction_Reset(C4_UI_Interaction* input);