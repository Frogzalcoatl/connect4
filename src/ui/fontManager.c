#include "Connect4/ui/fontManager.h"
#include <stdio.h>
#include "FONT_Monocraft.h"
#include "FONT_Monocraft_Bold.h"

#define C4_FONT_COUNT 2

static TTF_Font* fontCache[C4_FONT_COUNT] = {0};

typedef struct {
    const unsigned char* data;
    const size_t len;
} FontAsset;

static const FontAsset FONT_ASSETS[C4_FONT_COUNT] = {
    {FONT_Monocraft, FONT_Monocraft_len},
    {FONT_Monocraft_Bold, FONT_Monocraft_Bold_len}
};

TTF_Font* C4_GetFont(C4_FontType type) {
    if (type < 0 || type >= C4_FONT_COUNT) {
        return NULL;
    }
    if (fontCache[type] != NULL) {
        return fontCache[type];
    }
    SDL_IOStream* io = SDL_IOFromConstMem(FONT_ASSETS[type].data, FONT_ASSETS[type].len);
    if (!io) {
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