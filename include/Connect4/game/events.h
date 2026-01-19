#pragma once
#include <stdbool.h>
#include "Connect4/ui/screens/screen.h"
#include "Connect4/assets/sounds.h"
#include "Connect4/game/board.h"

typedef enum C4_EventType {
    C4_EVENT_NONE,
    C4_EVENT_CLOSE_WINDOW,
    C4_EVENT_SCREEN_CHANGE,
    C4_EVENT_SET_CURSOR,
    C4_EVENT_PLAY_SOUND,
    C4_EVENT_PLAY_MUSIC,
    C4_EVENT_SET_VOLUME
} C4_EventType;

typedef struct C4_Event {
    C4_EventType type;
    union {
        struct { C4_ScreenType type; } screenChange;
        struct { bool androidRemoveTask; } closeWindow;
        struct { SDL_SystemCursor type; } setCursor;
        struct { C4_SoundEffect id; } playSound;
        struct { C4_MusicTrack id; } playMusic;
        struct { C4_AudioTrack track; float level; } setVolume;
    };
} C4_Event;

void C4_InitEvents(void);
void C4_PushEvent(C4_Event event);
bool C4_PollEvent(C4_Event* event);
void C4_PushEvent_CloseWindow(bool androidRemoveTask);
void C4_PushEvent_ScreenChange(C4_ScreenType screenType);
void C4_PushEvent_SetCursor(SDL_SystemCursor type);
void C4_PushEvent_PlaySound(C4_SoundEffect id);
void C4_PushEvent_PlayMusic(C4_MusicTrack id);
void C4_PushEvent_SetVolume(C4_AudioTrack track, float level);