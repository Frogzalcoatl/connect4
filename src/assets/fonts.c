#include "Connect4/assets/fonts.h"
#include "Connect4/constants.h"
#include "Connect4/tools/virtualFileSystem.h"
#include <stdio.h>

#define C4_FONT_ASSET_COUNT 2
static const char* FONT_ASSET_PATHS[C4_FONT_ASSET_COUNT] = {
    "assets/fonts/Monocraft.ttf",
    "assets/fonts/Miracode.ttf"
};
typedef struct C4_FontData {
    void* data;
    size_t size;
} C4_FontData;
static C4_FontData fontFileBuffers[C4_FONT_ASSET_COUNT] = {0};

static void C4_LoadFontData(C4_FontAsset id) {
    if (!fontFileBuffers[id].data) {
        fontFileBuffers[id].data = C4_VFS_ReadFile(FONT_ASSET_PATHS[id], &fontFileBuffers[id].size);
    }
}

typedef struct {
    C4_FontAsset assetID;
    float size;
    TTF_FontStyleFlags style;
    TTF_Font* font;
    void* rawData;
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

    if (!fontFileBuffers[assetID].data) {
        C4_LoadFontData(assetID);
    }

    void* rawData = fontFileBuffers[assetID].data;
    if (!rawData) {
        SDL_Log("Unable to access font file");
        return NULL;
    }
    size_t len = fontFileBuffers[assetID].size;
    
    SDL_IOStream* io = SDL_IOFromMem(rawData, len);

    if (!io) {
        SDL_Log("Failed to create IO for font asset %d", assetID);
        C4_VFS_FreeFile(rawData);
        return NULL;
    }

    TTF_Font* newFont = TTF_OpenFontIO(io, true, ptSize);
    if (!newFont) {
        SDL_Log("Failed to get font asset %d: %s", assetID, SDL_GetError());
        C4_VFS_FreeFile(rawData);
        return NULL;
    }

    TTF_SetFontStyle(newFont, style);

    fontCache[cacheCount].assetID = assetID;
    fontCache[cacheCount].size = ptSize;
    fontCache[cacheCount].style = style;
    fontCache[cacheCount].font = newFont;
    fontCache[cacheCount].rawData = rawData;

    cacheCount++;
    
    return newFont;
}

void C4_CloseAllFonts(void) {
    for (size_t i = 0; i < MAX_FONT_CACHE; i++) {
        if (fontCache[i].font) {
            TTF_CloseFont(fontCache[i].font);
        }
        if (fontCache[i].rawData) {
            C4_VFS_FreeFile(fontCache[i].rawData);
        }
    }
    cacheCount = 0;
    memset(fontCache, 0, sizeof(fontCache));
}