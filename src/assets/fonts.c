#include "Connect4/assets/header.h"
#include "Connect4/assets/fonts.h"
#include "assets_fonts_Monocraft_ttf.h"
#include "assets_fonts_Miracode_ttf.h"
#include "Connect4/constants.h"
#include <stdio.h>

#define C4_FONT_ASSET_COUNT 2

static const C4_HeaderAsset FONT_ASSETS[C4_FONT_ASSET_COUNT] = {
    {assets_fonts_Monocraft_ttf_data, assets_fonts_Monocraft_ttf_size},
    {assets_fonts_Miracode_ttf_data, assets_fonts_Miracode_ttf_size}
};

typedef struct {
    C4_FontAsset assetID;
    float size;
    TTF_FontStyleFlags style;
    TTF_Font* font;
} C4_CachedFont;

#define MAX_FONT_CACHE 64
static C4_CachedFont fontCache[MAX_FONT_CACHE];
static int cacheCount = 0; 

TTF_Font* C4_GetFont(C4_FontAsset assetID, float ptSize, TTF_FontStyleFlags style) {
    if (assetID < 0 || assetID >= C4_FONT_ASSET_COUNT) {
        SDL_Log("Invalid Font Asset ID requested");
        return NULL;
    }

    if (ptSize <= 0.f) {
        ptSize = 12.f;
    }

    for (int i = 0; i < cacheCount; i++) {
        if (
            fontCache[i].assetID == assetID &&
            fontCache[i].size == ptSize &&
            fontCache[i].style == style
        ) {
            return fontCache[i].font;
        }
    }

    if (cacheCount >= MAX_FONT_CACHE) {
        SDL_Log("Font cache full. Increase MAX_FONT_CACHE");
        return fontCache[0].font;
    }

    const C4_HeaderAsset* asset = &FONT_ASSETS[assetID];
    SDL_IOStream* io = SDL_IOFromConstMem(asset->data, asset->size);

    if (!io) {
        SDL_Log("Failed to create IO for font asset %d", assetID);
        return NULL;
    }

    TTF_Font* newFont = TTF_OpenFontIO(io, true, ptSize);
    if (!newFont) {
        SDL_Log("Failed to get font asset %d: %s", assetID, SDL_GetError());
        return NULL;
    }

    TTF_SetFontStyle(newFont, style);

    fontCache[cacheCount].assetID = assetID;
    fontCache[cacheCount].size = ptSize;
    fontCache[cacheCount].style = style;
    fontCache[cacheCount].font = newFont;

    cacheCount++;
    
    return newFont;
}

void C4_CloseAllFonts(void) {
    for (size_t i = 0; i < MAX_FONT_CACHE; i++) {
        if (fontCache[i].font) {
            TTF_CloseFont(fontCache[i].font);
        }
    }
    cacheCount = 0;
    memset(fontCache, 0, sizeof(fontCache));
}