#pragma once
#include <stdbool.h>

typedef enum {
    C4_SoundEffect_ButtonClick,
    C4_SoundEffect_ButtonHover,
    C4_SoundEffect_Player1Place,
    C4_SoundEffect_Player2Place,
    C4_SoundEffect_ListSize
} C4_SoundEffect;

bool C4_InitAudio();
void C4_QuitAudio();
void C4_PlaySound(C4_SoundEffect soundID);