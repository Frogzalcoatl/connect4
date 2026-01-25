#include "Connect4/assets/sounds.h"
#include "Connect4/tools/virtualFileSystem.h"
#include "Connect4/game/consoleOutput.h"
#include <assert.h>

static const char* SOUND_ASSETS[C4_SoundEffect_Count] = {
    "assets/sounds/ButtonClick.ogg",
    "assets/sounds/ButtonHover.ogg",
    "assets/sounds/Player1Place.ogg",
    "assets/sounds/Player2Place.ogg"
};
static const char* MUSIC_ASSETS[C4_MusicTrack_Count] = {
    "assets/music/test.ogg"
};

static const char* SOUND_TAG = "SFX";
static const char* MUSIC_TAG = "Music";

C4_SoundSystem* C4_SoundSystem_Create(void) {
    C4_SoundSystem* soundSystem = SDL_calloc(1, sizeof(C4_SoundSystem));
    if (!soundSystem) {
        C4_Warn(
            SDL_LOG_CATEGORY_AUDIO,
            "Unable to init sound system. Pointer is NULL."
        );
        return NULL;
    }

    soundSystem->mixerDevice = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!soundSystem->mixerDevice) {
        C4_Warn(
            SDL_LOG_CATEGORY_AUDIO,
            "Mixer creation failed: %s",
            SDL_GetError()
        );
        return NULL;
    }

    size_t currentLen;

    for (size_t i = 0; i < C4_SoundEffect_Count; i++) {
        void* rawData = C4_VFS_ReadFile(SOUND_ASSETS[i], &currentLen);
        if (!rawData) {
            C4_Warn(
                SDL_LOG_CATEGORY_AUDIO,
                "Unable to read sound: %s",
                SOUND_ASSETS[i]
            );
            continue;
        }
        SDL_IOStream* io = SDL_IOFromMem(rawData, currentLen);
        if (!io) {
            C4_VFS_FreeFile(rawData);
            C4_Warn(
                SDL_LOG_CATEGORY_AUDIO,
                "Unable to create SDL io for sound: %s",
                SOUND_ASSETS[i]
            );
            continue;
        }
        soundSystem->loadedSounds[i] = MIX_LoadAudio_IO(soundSystem->mixerDevice, io, true, true);
        C4_VFS_FreeFile(rawData);
    }

    soundSystem->musicTrack = MIX_CreateTrack(soundSystem->mixerDevice);
    MIX_TagTrack(soundSystem->musicTrack, MUSIC_TAG);

    for (int i = 0; i < SFX_POOL_SIZE; i++) {
        soundSystem->sfxTracks[i] = MIX_CreateTrack(soundSystem->mixerDevice);
        MIX_TagTrack(soundSystem->sfxTracks[i], SOUND_TAG);
    }

    soundSystem->musicStartTime = 0;
    soundSystem->isMusicPlaying = false;

    C4_SoundSystem_SetVolume(soundSystem, C4_AudioTrack_Master, 1.0f);
    C4_SoundSystem_SetVolume(soundSystem, C4_AudioTrack_Music, 0.5f);
    C4_SoundSystem_SetVolume(soundSystem, C4_AudioTrack_Sound, 1.0f);

    C4_Log("C4 sound system setup complete");

    return soundSystem;
}


void C4_SoundSystem_Destroy(C4_SoundSystem* soundSystem) {
    if (!soundSystem) {
        return;
    }

    if (soundSystem->musicTrack) {
        MIX_SetTrackAudio(soundSystem->musicTrack, NULL);
        MIX_DestroyTrack(soundSystem->musicTrack);
        soundSystem->musicTrack = NULL;
    }
    for (int i = 0; i < SFX_POOL_SIZE; i++) {
        if (soundSystem->sfxTracks[i]) {
            MIX_SetTrackAudio(soundSystem->sfxTracks[i], NULL);
            MIX_DestroyTrack(soundSystem->sfxTracks[i]);
            soundSystem->sfxTracks[i] = NULL;
        }
    }

    for (size_t i = 0; i < C4_SoundEffect_Count; i++) {
        if (soundSystem->loadedSounds[i]) {
            MIX_DestroyAudio(soundSystem->loadedSounds[i]);
            soundSystem->loadedSounds[i] = NULL;
        }
    }
    if (soundSystem->currentMusic) {
        MIX_DestroyAudio(soundSystem->currentMusic);
        soundSystem->currentMusic = NULL;
    }

    if (soundSystem->mixerDevice) {
        MIX_DestroyMixer(soundSystem->mixerDevice);
        soundSystem->mixerDevice = NULL;
    }

    if (soundSystem->currentMusicData) {
        C4_VFS_FreeFile(soundSystem->currentMusicData);
        soundSystem->currentMusicData = NULL;
    }

    SDL_free(soundSystem);
}

void C4_SoundSystem_PlaySound(C4_SoundSystem* soundSystem, C4_SoundEffect soundID) {
    assert(soundID >= 0 && soundID < C4_SoundEffect_Count);
    if (!soundSystem) {
        return;
    }
    if (!soundSystem->loadedSounds[soundID]) {
        C4_Warn(
            SDL_LOG_CATEGORY_AUDIO,
            "Attempted to play unloaded sound: %s",
            SOUND_ASSETS[soundID]
        );
        return;
    }

    MIX_Track* freeTrack = NULL;
    for (int i = 0; i < SFX_POOL_SIZE; i++) {
        if (!MIX_TrackPlaying(soundSystem->sfxTracks[i])) {
            freeTrack = soundSystem->sfxTracks[i];
            break;
        }
    }

    if (!freeTrack) {
        freeTrack = soundSystem->sfxTracks[0]; 
    }

    MIX_SetTrackAudio(freeTrack, soundSystem->loadedSounds[soundID]);
    MIX_PlayTrack(freeTrack, 0);
}

void C4_SoundSystem_PlayMusic(C4_SoundSystem* soundSystem, C4_MusicTrack musicID) {
    assert(musicID >= 0 && musicID < C4_MusicTrack_Count);
    if (!soundSystem) {
        return;
    }

    MIX_PauseTrack(soundSystem->musicTrack);

    if (soundSystem->currentMusic) {
        MIX_DestroyAudio(soundSystem->currentMusic);
        soundSystem->currentMusic = NULL;
    }

    if (soundSystem->currentMusicData) {
        C4_VFS_FreeFile(soundSystem->currentMusicData);
        soundSystem->currentMusicData = NULL;
    }

    soundSystem->currentMusicID = C4_MusicTrack_None;

    size_t len;
    void* rawData = C4_VFS_ReadFile(MUSIC_ASSETS[musicID], &len);
    if (!rawData) {
        return;
    }

    SDL_IOStream* io = SDL_IOFromMem(rawData, len);
    if (!io) {
        C4_Warn(
            SDL_LOG_CATEGORY_AUDIO,
            "Failed to create IO for music: %s",
            MUSIC_ASSETS[musicID]
        );
        C4_VFS_FreeFile(rawData);
        return;
    }

    soundSystem->currentMusic = MIX_LoadAudio_IO(soundSystem->mixerDevice, io, false, true);

    if (!soundSystem->currentMusic) {
        C4_Warn(
            SDL_LOG_CATEGORY_AUDIO,
            "Mixer failed to load audio IO: %s",
            SDL_GetError()
        );
        C4_VFS_FreeFile(rawData);
        return;
    }

    soundSystem->currentMusicData = rawData;
    
    soundSystem->currentMusicID = musicID;
    
    MIX_SetTrackAudio(soundSystem->musicTrack, soundSystem->currentMusic);
    
    SDL_PropertiesID props = SDL_CreateProperties();
    // -1 is infinite loop
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    MIX_PlayTrack(soundSystem->musicTrack, props);
    SDL_DestroyProperties(props);

    soundSystem->musicStartTime = SDL_GetTicks(); 
    soundSystem->isMusicPlaying = true;
}

void C4_SoundSystem_SetVolume(C4_SoundSystem* soundSystem, C4_AudioTrack trackID, float volume) {
    if (!soundSystem || !soundSystem->mixerDevice) {
        C4_Warn(
            SDL_LOG_CATEGORY_AUDIO,
            "Unable to set volume. One or more required pointers are NULL"
        );
        return;
    }
    if (trackID == C4_AudioTrack_Master) {
        MIX_SetMixerGain(soundSystem->mixerDevice, volume);
        return;
    }

    assert(trackID == C4_AudioTrack_Sound || trackID == C4_AudioTrack_Music);

    const char* tag = NULL;
    if (trackID == C4_AudioTrack_Sound) {
        tag = SOUND_TAG;
    } else if (trackID == C4_AudioTrack_Music) {
        tag = MUSIC_TAG;
    } else {
        return;
    }
    MIX_SetTagGain(soundSystem->mixerDevice, tag, volume);
}

static double C4_GetMusicPosition(C4_SoundSystem* soundSystem) {
    if (!soundSystem) {
        C4_Warn(
            SDL_LOG_CATEGORY_AUDIO,
            "Unable to get music position. Sound system pointer is NULL"
        );
        return 0.0;
    }
    if (!soundSystem->isMusicPlaying) {
        return 0.0;
    }
    
    Uint64 now = SDL_GetTicks();
    return (double)(now - soundSystem->musicStartTime) / 1000.0;
}

float C4_GetBPMBounce(C4_SoundSystem* soundSystem, float bpm) {
    if (!soundSystem) {
        C4_Warn(
            SDL_LOG_CATEGORY_AUDIO,
            "Unable to get bpm bounce. Sound system pointer is NULL"
        );
        return 0.0f;
    }
    if (!soundSystem->isMusicPlaying) {
        return 0.0f;
    }

    double position = C4_GetMusicPosition(soundSystem);
    
    double frequency = (bpm / 60.0) * SDL_PI_D * 2.0;
    
    return (float)SDL_fabs(SDL_sin(position * frequency / 2.0)); 
}