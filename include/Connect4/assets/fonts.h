#pragma once
#include <SDL3_ttf/SDL_ttf.h>

typedef enum C4_FontAsset {
    C4_FONT_ASSET_MONOCRAFT,
    C4_FONT_ASSET_MIRACODE,
    C4_FONT_ASSET_COUNT
} C4_FontAsset;

TTF_Font* C4_GetFont(C4_FontAsset assetID, float ptSize, TTF_FontStyleFlags style);
void C4_CloseAllFonts(void);