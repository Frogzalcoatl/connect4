#include "Connect4/assets/fonts.h"
#include "Connect4/constants.h"
#include "Connect4/tools/virtualFileSystem.h"
#include "Connect4/game/consoleOutput.h"
#include "Connect4/tools/dynamicArray.h"
#include <stdio.h>
#include <assert.h>

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
    assert(id >= 0 && id < C4_FONT_ASSET_COUNT);

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
typedef struct C4_FontList {
    C4_CachedFont* data;
    size_t count;
    size_t capacity;
} C4_FontList;
static C4_FontList fontCache;

TTF_Font* C4_GetFont(C4_FontAsset assetID, float ptSize, TTF_FontStyleFlags style) {
    assert(assetID >= 0 && assetID < C4_FONT_ASSET_COUNT);

    if (ptSize <= 0.f) {
        ptSize = 12.f;
    }

    for (size_t i = 0; i < fontCache.count; i++) {
        if (
            fontCache.data[i].assetID == assetID &&
            fontCache.data[i].size == ptSize &&
            fontCache.data[i].style == style
        ) {
            return fontCache.data[i].font;
        }
    }

    if (!fontFileBuffers[assetID].data) {
        C4_LoadFontData(assetID);
    }

    void* rawData = fontFileBuffers[assetID].data;
    if (!rawData) {
        C4_FatalError(
            C4_ErrorCode_GenericRuntimeError,
            "Unable get load font %s",
            FONT_ASSET_PATHS[assetID]
        );
    }


    size_t len = fontFileBuffers[assetID].size;
    
    SDL_IOStream* io = SDL_IOFromMem(rawData, len);
    if (!io) {
        C4_FatalError(
            C4_ErrorCode_DependencyErrorSDL, 
            "SDL_IOFromMem failed: %s",
            SDL_GetError()
        );
    }

    TTF_Font* newFont = TTF_OpenFontIO(io, true, ptSize);
    if (!newFont) {
        C4_FatalError(
            C4_ErrorCode_DependencyErrorTTF, 
            "TTF_OpenFontIO failed: %s",
            SDL_GetError()
        );
    }

    TTF_SetFontStyle(newFont, style);

    C4_CachedFont newFontCache;
    newFontCache.assetID = assetID;
    newFontCache.size = ptSize;
    newFontCache.style = style;
    newFontCache.font = newFont;
    newFontCache.rawData = rawData;

    C4_DynamicArray_Push_Back(fontCache, newFontCache);
    
    return newFont;
}

void C4_CloseAllFonts(void) {
    for (size_t i = 0; i < fontCache.count; i++) {
        if (fontCache.data[i].font) {
            TTF_CloseFont(fontCache.data[i].font);
        }
    }
    
    C4_DynamicArray_Free(fontCache);

    for (size_t i = 0; i < C4_FONT_ASSET_COUNT; i++) {
        if (fontFileBuffers[i].data) {
            C4_VFS_FreeFile(fontFileBuffers[i].data);
            fontFileBuffers[i].data = NULL;
            fontFileBuffers[i].size = 0;
        }
    }

    C4_Log("Closed all fonts in cache");
}