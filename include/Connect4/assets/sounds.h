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

bool C4_InitAudio(void);
void C4_QuitAudio(void);
void C4_PlaySound(C4_SoundEffect soundID);