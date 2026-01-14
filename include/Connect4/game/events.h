#pragma once
#include <stdbool.h>
#include "Connect4/ui/screens/screen.h"
#include "Connect4/assets/sounds.h"
#include "Connect4/game/board.h"

typedef enum {
    C4_EVENT_NONE,
    C4_EVENT_CLOSE_WINDOW,
    C4_EVENT_SCREEN_CHANGE
} C4_EventType;

typedef struct {
    C4_EventType type;
    union {
        struct { C4_ScreenType type; } screenChange;
        struct { bool androidRemoveTask; } closeWindow;
    };
} C4_Event;

void C4_InitEvents(void);
void C4_PushEvent(C4_Event event);
bool C4_PollEvent(C4_Event* event);
void C4_PushEvent_CloseWindow(bool androidRemoveTask);
void C4_PushEvent_ScreenChange(C4_ScreenType screenType);