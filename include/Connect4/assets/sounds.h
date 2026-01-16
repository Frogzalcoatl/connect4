#pragma once
#include <stdbool.h>

typedef enum {
    C4_SoundEffect_None = -1,
    C4_SoundEffect_ButtonClick,
    C4_SoundEffect_ButtonHover,
    C4_SoundEffect_Player1Place,
    C4_SoundEffect_Player2Place,
    C4_SoundEffect_Count
} C4_SoundEffect;

typedef enum {
    C4_MusicTrack_None = -1,
    C4_MusicTrack_Test,
    C4_MusicTrack_Count
} C4_MusicTrack;

bool C4_InitAudio(void);
void C4_QuitAudio(void);
void C4_PlaySound(C4_SoundEffect soundID);
void C4_PlayMusic(C4_MusicTrack musicID);

// Range 0.0f-1.0f
void C4_SetMusicVolume(float volume);
void C4_SetSoundVolume(float volume);
void C4_SetMasterVolume(float volume);

float C4_GetBPMBounce(float bpm);