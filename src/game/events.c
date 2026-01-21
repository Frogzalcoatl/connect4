#include "Connect4/game/events.h"
#include "Connect4/game/consoleOutput.h"
#include "SDL3/SDL.h"
#include <assert.h>

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
        C4_Log("Event Queue Overflow. Dropping event type %d", event.type);
        return;
    }
    eventQueue[head] = event;
    head = nextHead;
}

bool C4_PollEvent(C4_Event* event) {
    assert(event);

    if (head == tail) {
        // Event queue is empty
        return false;
    }
    *event = eventQueue[tail];
    tail = (tail + 1) % C4_EVENT_QUEUE_SIZE;
    return true;
}

void C4_PushEvent_CloseWindow(bool androidRemoveTask) {
    C4_Event event = {0};
    event.type = C4_EVENT_CLOSE_WINDOW;
    event.closeWindow.androidRemoveTask = androidRemoveTask;
    C4_PushEvent(event);
}

void C4_PushEvent_ScreenChange(C4_ScreenType screenType) {
    assert(screenType >= 0 && screenType < C4_ScreenType_ScreenCount);

    C4_Event event = {0};
    event.type = C4_EVENT_SCREEN_CHANGE;
    event.screenChange.type = screenType;
    C4_PushEvent(event);
}

void C4_PushEvent_SetCursor(SDL_SystemCursor type) {
    assert(type >= SDL_SYSTEM_CURSOR_DEFAULT && type < SDL_SYSTEM_CURSOR_COUNT);

    C4_Event event = {0};
    event.type = C4_EVENT_SET_CURSOR;
    event.setCursor.type = type;
    C4_PushEvent(event);
}

void C4_PushEvent_PlaySound(C4_SoundEffect id) {
    assert(id >= 0 && id < C4_SoundEffect_Count);

    C4_Event event = {0};
    event.type = C4_EVENT_PLAY_SOUND;
    event.playSound.id = id;
    C4_PushEvent(event);
}

void C4_PushEvent_PlayMusic(C4_MusicTrack id) {
    assert(id >= 0 && id < C4_MusicTrack_Count);
    C4_Event event = {0};
    event.type = C4_EVENT_PLAY_MUSIC;
    event.playMusic.id = id;
    C4_PushEvent(event);
}

void C4_PushEvent_SetVolume(C4_AudioTrack track, float level) {
    assert(level >= 0.0f && level <= 1.5f);
    C4_Event event = {0};
    event.type = C4_EVENT_SET_VOLUME;
    event.setVolume.track = track;
    event.setVolume.level = level;
    C4_PushEvent(event);
}