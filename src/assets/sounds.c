#include <SDL3/SDL.h>
#include "Connect4/assets/index.h"
#include "Connect4/assets/sounds.h"
#include "assets_sounds_ButtonClick_wav.h"
#include "assets_sounds_ButtonHover_wav.h"
#include "assets_sounds_Player1Place_wav.h"
#include "assets_sounds_Player2Place_wav.h"

static const HeaderAsset SOUND_ASSETS[C4_SoundEffect_ListSize] = {
    {assets_sounds_ButtonClick_wav_data, assets_sounds_ButtonClick_wav_size},
    {assets_sounds_ButtonHover_wav_data, assets_sounds_ButtonHover_wav_size},
    {assets_sounds_Player1Place_wav_data, assets_sounds_Player1Place_wav_size},
    {assets_sounds_Player2Place_wav_data, assets_sounds_Player2Place_wav_size}
};
static SDL_AudioDeviceID deviceID;

static SDL_AudioSpec specs[C4_SoundEffect_ListSize];
static Uint8* buffers[C4_SoundEffect_ListSize];
static Uint32 lengths[C4_SoundEffect_ListSize];

#define MAX_STREAMS 8
static SDL_AudioStream* streams[MAX_STREAMS];
static int currentStreamIndex = 0;

bool C4_InitAudio() {
    deviceID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (deviceID == 0) {
        SDL_Log("Failed to get audio deviceID");
        return false;
    }
    for (size_t i = 0; i < C4_SoundEffect_ListSize; i++) {
        SDL_IOStream* io = SDL_IOFromConstMem(SOUND_ASSETS[i].data, SOUND_ASSETS[i].size);
        if (!io) {
            SDL_Log("Failed to create IO for sound index: %zu", i);
            C4_QuitAudio();
            return false;
        }
        if (!SDL_LoadWAV_IO(io, true, &specs[i], &buffers[i], &lengths[i])) {
            SDL_Log("Failed to load sound index: %zu", i);
            C4_QuitAudio();
            return false;
        }
    }
    for (size_t i = 0; i < MAX_STREAMS; i++) {
        // NULL means use device defaults
        streams[i] = SDL_CreateAudioStream(NULL, NULL);
        if (!streams[i]) {
            SDL_Log("Failed to load audio stream index: %zu", i);
            C4_QuitAudio();
            return false;
        }
        SDL_BindAudioStream(deviceID, streams[i]);
    }
    return true;
}

void C4_QuitAudio() {
    for (size_t i = 0; i < C4_SoundEffect_ListSize; i++) {
        if (buffers[i]) {
            SDL_free(buffers[i]);
            buffers[i] = NULL;
        }
    }
    for (size_t i = 0; i < MAX_STREAMS; i++) {
        if (streams[i]) {
            SDL_DestroyAudioStream(streams[i]);
            streams[i] = NULL;
        }
    }
    if (deviceID != 0) {
        SDL_CloseAudioDevice(deviceID);
    }
}

void C4_PlaySound(C4_SoundEffect soundID) {
    if (soundID < 0 || soundID >= C4_SoundEffect_ListSize || deviceID == 0) {
        return;
    }
    SDL_AudioStream* stream = streams[currentStreamIndex];
    currentStreamIndex = (currentStreamIndex + 1) % MAX_STREAMS;
    SDL_SetAudioStreamFormat(stream, &specs[soundID], NULL);
    SDL_PutAudioStreamData(stream, buffers[soundID], lengths[soundID]);
}