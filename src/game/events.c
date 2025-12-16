#include "Connect4/game/events.h"
#include <SDL3/SDL.h>

#define C4_EVENT_QUEUE_SIZE 256

static C4_Event eventQueue[C4_EVENT_QUEUE_SIZE];
static int head = 0;
static int tail = 0;

void C4_InitEvents() {
    head = 0;
    tail = 0;
}

void C4_PushEvent(C4_Event event) {
    int nextHead = (head + 1) % C4_EVENT_QUEUE_SIZE;
    if (nextHead == tail) {
        SDL_Log("Event Queue Overflow. Dropping event type %d", event.type);
        return;
    }
    eventQueue[head] = event;
    head = nextHead;
}

bool C4_PollEvent(C4_Event* event) {
    if (head == tail) {
        return false;
    }
    *event = eventQueue[tail];
    tail = (tail + 1) % C4_EVENT_QUEUE_SIZE;
    return true;
}

void C4_PushEvent_CloseWindow() {
    C4_Event event;
    event.type = C4_EVENT_CLOSE_WINDOW;
    C4_PushEvent(event);
}

void C4_PushEvent_ScreenChange(C4_ScreenType screenType) {
    C4_Event event;
    event.type = C4_EVENT_SCREEN_CHANGE;
    event.screenChange.type = screenType;
    C4_PushEvent(event);
}

void C4_PushEvent_GameOver(C4_SlotState winnerId) {
    C4_Event event;
    event.type = C4_EVENT_GAME_OVER;
    event.gameOver.winnerId = winnerId;
    C4_PushEvent(event);
}

void C4_PushEvent_SoundRequest(C4_SoundEffect soundId) {
    C4_Event event;
    event.type = C4_EVENT_SOUND_REQUEST;
    event.sound.id = soundId;
    C4_PushEvent(event);
}