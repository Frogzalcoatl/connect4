#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "Connect4/assets/sounds.h"
#include "Connect4/tools/virtualFileSystem.h"
#include <math.h>

// The max amount of sounds that can play at once
#define SFX_POOL_SIZE 8

static const char* SOUND_ASSETS[C4_SoundEffect_Count] = {
    "assets/sounds/ButtonClick.wav",
    "assets/sounds/ButtonHover.wav",
    "assets/sounds/Player1Place.wav",
    "assets/sounds/Player2Place.wav"
};

static const char* MUSIC_ASSETS[C4_MusicTrack_Count] = {
    "assets/music/test.mp3"
};

static MIX_Mixer* mixerDevice = NULL;

static MIX_Audio* loadedSounds[C4_SoundEffect_Count] = {0};

static MIX_Audio* currentMusic = NULL;
static C4_MusicTrack currentMusicID = C4_MusicTrack_None;

static MIX_Track* musicTrack = NULL;
static MIX_Track* sfxTracks[SFX_POOL_SIZE] = {0};

static Uint64 musicStartTime = 0;
static bool isMusicPlaying = false;

bool C4_InitAudio(void) {   
    MIX_Init(); 

    mixerDevice = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!mixerDevice) {
        SDL_Log("Mixer creation failed: %s", SDL_GetError());
        return false;
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
            C4_QuitAudio();
            return false;
        }
        loadedSounds[i] = MIX_LoadAudio_IO(mixerDevice, io, true, true);
        C4_VFS_FreeFile(rawData);
    }

    musicTrack = MIX_CreateTrack(mixerDevice);
    MIX_TagTrack(musicTrack, "Music");

    for (int i = 0; i < SFX_POOL_SIZE; i++) {
        sfxTracks[i] = MIX_CreateTrack(mixerDevice);
        MIX_TagTrack(sfxTracks[i], "SFX");
    }

    C4_SetMasterVolume(1.0f);
    C4_SetMusicVolume(0.5f);
    C4_SetSoundVolume(1.0f);

    return true;
}


void C4_QuitAudio(void) {
    if (musicTrack) {
        MIX_DestroyTrack(musicTrack);
        musicTrack = NULL;
    }
    for (int i = 0; i < SFX_POOL_SIZE; i++) {
        if (sfxTracks[i]) {
            MIX_DestroyTrack(sfxTracks[i]);
            sfxTracks[i] = NULL;
        }
    }
    for (size_t i = 0; i < C4_SoundEffect_Count; i++) {
        if (loadedSounds[i]) {
            MIX_DestroyAudio(loadedSounds[i]);
            loadedSounds[i] = NULL;
        }
    }
    if (currentMusic) {
        MIX_DestroyAudio(currentMusic);
        currentMusic = NULL;
    }
    if (mixerDevice) {
        MIX_DestroyMixer(mixerDevice);
        mixerDevice = NULL;
    }

    MIX_Quit();
}

void C4_PlaySound(C4_SoundEffect soundID) {
    if (
        soundID < 0 ||
        soundID >= C4_SoundEffect_Count ||
        !loadedSounds[soundID]
    ) {
        return;
    }

    MIX_Track* freeTrack = NULL;
    for (int i = 0; i < SFX_POOL_SIZE; i++) {
        if (!MIX_TrackPlaying(sfxTracks[i])) {
            freeTrack = sfxTracks[i];
            break;
        }
    }

    if (!freeTrack) {
        freeTrack = sfxTracks[0]; 
    }

    MIX_SetTrackAudio(freeTrack, loadedSounds[soundID]);
    MIX_PlayTrack(freeTrack, 0);
}

void C4_PlayMusic(C4_MusicTrack musicID) {
    if (musicID == currentMusicID && currentMusic != NULL) {
        return;
    }

    MIX_PauseTrack(musicTrack);
    if (currentMusic) {
        MIX_DestroyAudio(currentMusic);
        currentMusic = NULL;
    }
    currentMusicID = C4_MusicTrack_None;

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
    currentMusic = MIX_LoadAudio_IO(mixerDevice, io, false, true);

    C4_VFS_FreeFile(rawData);
    
    if (currentMusic) {
        currentMusicID = musicID;
        
        MIX_SetTrackAudio(musicTrack, currentMusic);
        
        SDL_PropertiesID props = SDL_CreateProperties();
        // -1 is infinite loop
        SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
        MIX_PlayTrack(musicTrack, props);
        SDL_DestroyProperties(props);

        musicStartTime = SDL_GetTicks(); 
        isMusicPlaying = true;
    } else {
        SDL_Log("Failed to load music audio: %s", SDL_GetError());
    }
}

void C4_SetMasterVolume(float volume) {
    if (mixerDevice) {
        MIX_SetMixerGain(mixerDevice, volume);
    }
}

void C4_SetMusicVolume(float volume) {
    if (mixerDevice) {
        MIX_SetTagGain(mixerDevice, "Music", volume);
    }
}

void C4_SetSoundVolume(float volume) {
    if (mixerDevice) {
        MIX_SetTagGain(mixerDevice, "SFX", volume);
    }
}

static double C4_GetMusicPosition(void) {
    if (!isMusicPlaying) {
        return 0.0;
    }
    
    Uint64 now = SDL_GetTicks();
    return (double)(now - musicStartTime) / 1000.0;
}

float C4_GetBPMBounce(float bpm) {
    if (!isMusicPlaying) {
        return 0.0f;
    }

    double position = C4_GetMusicPosition();
    
    double frequency = (bpm / 60.0) * M_PI * 2.0;
    
    return (float)fabs(sin(position * frequency / 2.0)); 
}