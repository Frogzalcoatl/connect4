#include "Connect4/game/events.h"
#include "SDL3/SDL.h"

#define C4_EVENT_QUEUE_SIZE 256

static C4_Event eventQueue[C4_EVENT_QUEUE_SIZE];
static int head = 0;
static int tail = 0;

void C4_InitEvents(void) {
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
        // Event queue is empty
        return false;
    }
    *event = eventQueue[tail];
    tail = (tail + 1) % C4_EVENT_QUEUE_SIZE;
    return true;
}

void C4_PushEvent_CloseWindow(bool androidRemoveTask) {
    C4_Event event;
    event.type = C4_EVENT_CLOSE_WINDOW;
    event.closeWindow.androidRemoveTask = androidRemoveTask;
    C4_PushEvent(event);
}

void C4_PushEvent_ScreenChange(C4_ScreenType screenType) {
    C4_Event event;
    event.type = C4_EVENT_SCREEN_CHANGE;
    event.screenChange.type = screenType;
    C4_PushEvent(event);
}

void C4_PushEvent_SetCursor(SDL_SystemCursor type) {
    C4_Event event;
    event.type = C4_EVENT_SET_CURSOR;
    event.setCursor.type = type;
    C4_PushEvent(event);
}

void C4_PushEvent_PlaySound(C4_SoundEffect id) {
    C4_Event event;
    event.type = C4_EVENT_PLAY_SOUND;
    event.playSound.id = id;
    C4_PushEvent(event);
}

void C4_PushEvent_PlayMusic(C4_MusicTrack id) {
    C4_Event event;
    event.type = C4_EVENT_PLAY_MUSIC;
    event.playMusic.id = id;
    C4_PushEvent(event);
}

void C4_PushEvent_SetVolume(C4_AudioTrack track, float level) {
    C4_Event event;
    event.type = C4_EVENT_SET_VOLUME;
    event.setVolume.track = track;
    event.setVolume.level = level;
    C4_PushEvent(event);
}