#include "Connect4/assets/sounds.h"
#include "Connect4/tools/virtualFileSystem.h"
#include <math.h>

static const char* SOUND_ASSETS[C4_SoundEffect_Count] = {
    "assets/sounds/ButtonClick.wav",
    "assets/sounds/ButtonHover.wav",
    "assets/sounds/Player1Place.wav",
    "assets/sounds/Player2Place.wav"
};
static const char* SOUND_TAG = "SFX";

static const char* MUSIC_ASSETS[C4_MusicTrack_Count] = {
    "assets/music/test.mp3"
};
static const char* MUSIC_TAG = "Music";

C4_SoundSystem* C4_SoundSystem_Create(void) {
    C4_SoundSystem* soundSystem = SDL_calloc(1, sizeof(C4_SoundSystem));
    if (!soundSystem) {
        SDL_Log("Unable to init sound system. Pointer is NULL.");
        return NULL;
    }

    soundSystem->mixerDevice = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!soundSystem->mixerDevice) {
        SDL_Log("Mixer creation failed: %s", SDL_GetError());
        return NULL;
    }

    size_t currentLen;

    for (size_t i = 0; i < C4_SoundEffect_Count; i++) {
        void* rawData = C4_VFS_ReadFile(SOUND_ASSETS[i], &currentLen);
        if (!rawData) {
            SDL_Log("Unable to open sound %s", SOUND_ASSETS[i]);
            continue;
        }
        SDL_IOStream* io = SDL_IOFromMem(rawData, currentLen);
        if (!io) {
            C4_VFS_FreeFile(rawData);
            C4_SoundSystem_Destroy(soundSystem);
            return NULL;
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

    return soundSystem;
}


void C4_SoundSystem_Destroy(C4_SoundSystem* soundSystem) {
    if (!soundSystem) {
        SDL_Log("Unable to destroy sound system. Pointer is NULL");
        return;
    }
    if (soundSystem->musicTrack) {
        MIX_DestroyTrack(soundSystem->musicTrack);
        soundSystem->musicTrack = NULL;
    }
    for (int i = 0; i < SFX_POOL_SIZE; i++) {
        if (soundSystem->sfxTracks[i]) {
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
    SDL_free(soundSystem);
}

void C4_SoundSystem_PlaySound(C4_SoundSystem* soundSystem, C4_SoundEffect soundID) {
    if (
        soundID < 0 ||
        soundID >= C4_SoundEffect_Count ||
        !soundSystem->loadedSounds[soundID]
    ) {
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
    if (musicID == soundSystem->currentMusicID && soundSystem->currentMusic != NULL) {
        return;
    }

    MIX_PauseTrack(soundSystem->musicTrack);
    if (soundSystem->currentMusic) {
        MIX_DestroyAudio(soundSystem->currentMusic);
        soundSystem->currentMusic = NULL;
    }
    soundSystem->currentMusicID = C4_MusicTrack_None;

    if (musicID < 0 || musicID >= C4_MusicTrack_Count) {
        return;
    }

    size_t len;
    void* rawData = C4_VFS_ReadFile(MUSIC_ASSETS[musicID], &len);
    if (!rawData) {
        SDL_Log("Unable to open music %s", MUSIC_ASSETS[musicID]);
        return;
    }
    SDL_IOStream* io = SDL_IOFromMem(rawData, len);
    if (!io) {
        SDL_Log("Failed to create IO for music %s", MUSIC_ASSETS[musicID]);
        return;
    }

    // Keep the mp3 compressed in RAM to save memory.
    // It will decompress in chunks during playback instead.
    soundSystem->currentMusic = MIX_LoadAudio_IO(soundSystem->mixerDevice, io, false, true);

    C4_VFS_FreeFile(rawData);
    
    if (soundSystem->currentMusic) {
        soundSystem->currentMusicID = musicID;
        
        MIX_SetTrackAudio(soundSystem->musicTrack, soundSystem->currentMusic);
        
        SDL_PropertiesID props = SDL_CreateProperties();
        // -1 is infinite loop
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
        MIX_PlayTrack(soundSystem->musicTrack, props);
        SDL_DestroyProperties(props);

        soundSystem->musicStartTime = SDL_GetTicks(); 
        soundSystem->isMusicPlaying = true;
    } else {
        SDL_Log("Failed to load music audio: %s", SDL_GetError());
    }
}

void C4_SoundSystem_SetVolume(C4_SoundSystem* soundSystem, C4_AudioTrack track, float volume) {
    if (!soundSystem || !soundSystem->mixerDevice) {
        SDL_Log("Unable to set volume. One or more required pointers are NULL");
        return;
    }
    if (track == C4_AudioTrack_Master) {
        MIX_SetMixerGain(soundSystem->mixerDevice, volume);
        return;
    }
    const char* tag;
    if (track == C4_AudioTrack_Sound) {
        tag = SOUND_TAG;
    } else if (track == C4_AudioTrack_Music) {
        tag = MUSIC_TAG;
    } else {
        return;
    }
    MIX_SetTagGain(soundSystem->mixerDevice, tag, volume);
}

static double C4_GetMusicPosition(C4_SoundSystem* soundSystem) {
    if (!soundSystem) {
        SDL_Log("Unable to get music position. Sound system pointer is NULL");
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
        SDL_Log("Unable to get bpm bounce. soundSystem is NULL");
        return 0.0f;
    }
    if (!soundSystem->isMusicPlaying) {
        return 0.0f;
    }

    double position = C4_GetMusicPosition(soundSystem);
    
    double frequency = (bpm / 60.0) * M_PI * 2.0;
    
    return (float)fabs(sin(position * frequency / 2.0)); 
}