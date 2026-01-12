#include "Connect4/assets/header.h"
#include "Connect4/assets/fonts.h"
#include "assets_fonts_Monocraft_ttf.h"
#include "assets_fonts_Miracode_ttf.h"
#include "Connect4/constants.h"
#include <stdio.h>

#define C4_TTF_COUNT 2
#define C4_FONT_COUNT C4_TTF_COUNT * 2

static TTF_Font* fontCache[C4_FONT_COUNT] = {0};

static const C4_HeaderAsset FONT_ASSETS[C4_TTF_COUNT] = {
    {assets_fonts_Monocraft_ttf_data, assets_fonts_Monocraft_ttf_size},
    {assets_fonts_Miracode_ttf_data, assets_fonts_Miracode_ttf_size}
};

TTF_Font* C4_GetFont(C4_FontType type) {
    if (type < 0 || type >= C4_FONT_COUNT) {
        SDL_Log("Tried to access invalid C4_FontType");
        return NULL;
    }
    if (fontCache[type] != NULL) {
        return fontCache[type];
    }
    const int ASSET_INDEX = type / 2;
    SDL_IOStream* io = SDL_IOFromConstMem(FONT_ASSETS[ASSET_INDEX].data, FONT_ASSETS[ASSET_INDEX].size);
    if (!io) {
        SDL_Log("Unable to open font IOStream. Asset index: %i", ASSET_INDEX);
        return NULL;
    }
    TTF_Font* newFont = TTF_OpenFontIO(io, true, 64.f);
    if (type % 2 == 1) {
        TTF_SetFontStyle(newFont, TTF_STYLE_BOLD);
    }
    if (newFont) {
        fontCache[type] = newFont;
        return newFont;
    }
    if (type != C4_FontType_Monocraft_Regular) {
        return C4_GetFont(C4_FontType_Monocraft_Regular);
    }
    SDL_Log("Unable to get font. Type index: %i", type);

    return NULL;
}

void C4_CloseAllFonts(void) {
    for (size_t i = 0; i < C4_FONT_COUNT; i++) {
        if (fontCache[i]) {
            TTF_CloseFont(fontCache[i]);
            fontCache[i] = NULL;
        }
    }
}