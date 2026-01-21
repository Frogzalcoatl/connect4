#pragma once
#include <stdbool.h>
#include "SDL3_mixer/SDL_mixer.h"

typedef enum C4_SoundEffect {
    C4_SoundEffect_None = -1,
    C4_SoundEffect_ButtonClick,
    C4_SoundEffect_ButtonHover,
    C4_SoundEffect_Player1Place,
    C4_SoundEffect_Player2Place,
    C4_SoundEffect_Count
} C4_SoundEffect;

typedef enum C4_MusicTrack {
    C4_MusicTrack_None = -1,
    C4_MusicTrack_Test,
    C4_MusicTrack_Count
} C4_MusicTrack;

// The max amount of sounds that can play at once
#define SFX_POOL_SIZE 8

typedef struct C4_SoundSystem {
    MIX_Mixer* mixerDevice;
    MIX_Audio* loadedSounds[C4_SoundEffect_Count];
    MIX_Audio* currentMusic;
    C4_MusicTrack currentMusicID;
    MIX_Track* musicTrack;
    void* currentMusicData; 
    MIX_Track* sfxTracks[SFX_POOL_SIZE];
    Uint64 musicStartTime;
    bool isMusicPlaying;
} C4_SoundSystem;

C4_SoundSystem* C4_SoundSystem_Create(void);
void C4_SoundSystem_Destroy(C4_SoundSystem* soundSystem);
void C4_SoundSystem_PlaySound(C4_SoundSystem* soundSystem, C4_SoundEffect soundID);
void C4_SoundSystem_PlayMusic(C4_SoundSystem* soundSystem, C4_MusicTrack musicID);

typedef enum C4_AudioTrack {
    C4_AudioTrack_Master,
    C4_AudioTrack_Sound,
    C4_AudioTrack_Music
} C4_AudioTrack;

// Range 0.0f-1.0f
void C4_SoundSystem_SetVolume(C4_SoundSystem* soundSystem, C4_AudioTrack trackID, float volume);

float C4_GetBPMBounce(C4_SoundSystem* soundSystem, float bpm);