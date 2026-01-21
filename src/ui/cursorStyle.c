#include "Connect4/ui/cursorStyle.h"
#include "Connect4/game/consoleOutput.h"
#include <assert.h>
#include <stddef.h>

// cursorCache's memory is guaranteed to be filled with zeros before main even runs
// This would happen regardless of whether " = {0}" is included since its static
static SDL_Cursor* cursorCache[SDL_SYSTEM_CURSOR_COUNT] = {0};

SDL_Cursor* C4_GetSystemCursor(SDL_SystemCursor type) {
    assert(type >= 0 && type < SDL_SYSTEM_CURSOR_COUNT);

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
    C4_Warn(
        SDL_LOG_CATEGORY_APPLICATION,
        "Unable to create system cursor"
    );
    return NULL;
}

void C4_DestroyAllCursors(void) {
    // Theoretically this fixes issues on linux
    SDL_SetCursor(NULL);
    for (size_t i = 0; i < SDL_SYSTEM_CURSOR_COUNT; i++) {
        if (cursorCache[i]) {
            SDL_DestroyCursor(cursorCache[i]);
            cursorCache[i] = NULL;
        }
    }

    C4_Log("Destroyed all system cursors in cache");
}