#include "Connect4/assets/index.h"
#include "Connect4/assets/fonts.h"
#include "assets_fonts_Monocraft_ttf.h"
#include "assets_fonts_Monocraft_Bold_ttf.h"
#include <stdio.h>

#define C4_FONT_COUNT 2

static TTF_Font* fontCache[C4_FONT_COUNT] = {0};

static const C4_HeaderAsset FONT_ASSETS[C4_FONT_COUNT] = {
    {assets_fonts_Monocraft_ttf_data, assets_fonts_Monocraft_ttf_size},
    {assets_fonts_Monocraft_Bold_ttf_data, assets_fonts_Monocraft_Bold_ttf_size}
};

TTF_Font* C4_GetFont(C4_FontType type) {
    if (type < 0 || type >= C4_FONT_COUNT) {
        SDL_Log("Tried to access invalid C4_FontType");
        return NULL;
    }
    if (fontCache[type] != NULL) {
        return fontCache[type];
    }
    SDL_IOStream* io = SDL_IOFromConstMem(FONT_ASSETS[type].data, FONT_ASSETS[type].size);
    if (!io) {
        SDL_Log("Unable to open font IOStream. Type index: %i", type);
        return NULL;
    }
    TTF_Font* newFont = TTF_OpenFontIO(io, true, 32);
    if (newFont) {
        fontCache[type] = newFont;
        return newFont;
    }
    if (type != C4_FontType_Regular) {
        return C4_GetFont(C4_FontType_Regular);
    }
    SDL_Log("Unable to get font. Type index: %i", type);
    return NULL;
}

void C4_CloseAllFonts() {
    for (size_t i = 0; i < C4_FONT_COUNT; i++) {
        if (fontCache[i]) {
            TTF_CloseFont(fontCache[i]);
            fontCache[i] = NULL;
        }
    }
}