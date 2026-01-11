#pragma once
#include <SDL3_ttf/SDL_ttf.h>

typedef enum {
    C4_FontType_Monocraft_Regular,
    C4_FontType_Monocraft_Bold,
    C4_FontType_Miracode_Regular,
    C4_FontType_Miracode_Bold
} C4_FontType;

TTF_Font* C4_GetFont(C4_FontType type);
void C4_CloseAllFonts(void);