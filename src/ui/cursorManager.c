#include "Connect4/ui/cursorManager.h"

// cursorCache's memory is guaranteed to be filled with zeros before main even runs
static SDL_Cursor* cursorCache[SDL_SYSTEM_CURSOR_COUNT] = {0};

SDL_Cursor* C4_GetSystemCursor(SDL_SystemCursor type) {
    if (type < 0 || type >= SDL_SYSTEM_CURSOR_COUNT) {
        return C4_GetSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    }
    if (cursorCache[type] != NULL) {
        return cursorCache[type];
    }
    SDL_Cursor* newCursor = SDL_CreateSystemCursor(type);
    if (newCursor) {
        cursorCache[type] = newCursor;
        return newCursor;
    }
    if (type != SDL_SYSTEM_CURSOR_DEFAULT) {
        return C4_GetSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    }
    return NULL;
}

void C4_DestroyAllCursors() {
    for (size_t i = 0; i < SDL_SYSTEM_CURSOR_COUNT; i++) {
        if (cursorCache[i]) {
            SDL_DestroyCursor(cursorCache[i]);
            cursorCache[i] = NULL;
        }
    }
}