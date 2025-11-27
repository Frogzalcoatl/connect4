#include "Connect4/ui/fontManager.h"
#include <stddef.h>
#include <stdio.h>

#define C4_FONT_COUNT 2
static TTF_Font* fontCache[C4_FONT_COUNT] = {0};
static const char* FONTPATH_MAP[C4_FONT_COUNT] = {
    "assets/fonts/Monocraft.ttf",
    "assets/fonts/Monocraft-Bold.ttf"
};

TTF_Font* C4_GetFont(C4_FontType type) {
    if (type < 0 || type >= C4_FONT_COUNT) {
        return NULL;
    }
    if (fontCache[type] != NULL) {
        return fontCache[type];
    }
    const char* basePath = SDL_GetBasePath();
    if (!basePath) {
        return NULL;
    }
    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s%s", basePath, FONTPATH_MAP[type]);
    TTF_Font* newFont = TTF_OpenFont(fullPath, 32.f);
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