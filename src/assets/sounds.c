#include "SDL3/SDL.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "Connect4/assets/header.h"
#include "Connect4/assets/sounds.h"

#include "assets_sounds_ButtonClick_wav.h"
#include "assets_sounds_ButtonHover_wav.h"
#include "assets_sounds_Player1Place_wav.h"
#include "assets_sounds_Player2Place_wav.h"
#include "assets_music_test_mp3.h"

#include <math.h>

// The max amount of sounds that can play at once
#define SFX_POOL_SIZE 8

static const C4_HeaderAsset SOUND_ASSETS[C4_SoundEffect_Count] = {
    {assets_sounds_ButtonClick_wav_data, assets_sounds_ButtonClick_wav_size},
    {assets_sounds_ButtonHover_wav_data, assets_sounds_ButtonHover_wav_size},
    {assets_sounds_Player1Place_wav_data, assets_sounds_Player1Place_wav_size},
    {assets_sounds_Player2Place_wav_data, assets_sounds_Player2Place_wav_size}
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
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        return false;
    }
    
    MIX_Init(); 

    mixerDevice = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!mixerDevice) {
        SDL_Log("Mixer creation failed: %s", SDL_GetError());
        return false;
    }

    for (size_t i = 0; i < C4_SoundEffect_Count; i++) {
        SDL_IOStream* io = SDL_IOFromConstMem(SOUND_ASSETS[i].data, SOUND_ASSETS[i].size);
        if (!io) {
            C4_QuitAudio();
            return false;
        }
        loadedSounds[i] = MIX_LoadAudio_IO(mixerDevice, io, true, true);
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

    SDL_IOStream* io = SDL_IOFromConstMem(assets_music_test_mp3_data, assets_music_test_mp3_size);
    if (!io) {
        SDL_Log("Failed to create IO for music ID %d", musicID);
        return;
    }

    // Keep the mp3 compressed in RAM to save memory.
    // It will decompress in chunks during playback instead.
    currentMusic = MIX_LoadAudio_IO(mixerDevice, io, false, true);
    
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