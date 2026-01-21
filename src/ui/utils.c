#include "Connect4/ui/utils.h"
#include "SDL3/SDL.h"
#include "Connect4/game/consoleOutput.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

int C4_Max(int a, int b) {
    return a > b ? a : b;
}

int C4_Min(int a, int b) {
    return a < b ? a : b;
}

float C4_FMax(float a, float b) {
    return a > b ? a : b;
}

float C4_FMin(float a, float b) {
    return a < b ? a : b;
}

size_t C4_ULLMax(size_t a, size_t b) {
    return a > b ? a : b;
}

size_t C4_ULLMin(size_t a, size_t b) {
    return a < b ? a : b;
}

// Vibe debugged ah function. Mine was extremely broken
// Remember to free values returned by this func
char* C4_JoinStrings(const char* strings[], size_t count, const char* separator) {
    assert(strings && count > 0 && separator);

    size_t separatorLength = (separator) ? strlen(separator) : 0;
    // Start with 1 for \0
    size_t totalLength = 1;
    size_t validCount = 0;

    for (size_t i = 0; i < count; i++) {
        if (strings[i]) {
            totalLength += strlen(strings[i]);
            validCount++;
        }
    }

    if (validCount == 0) {
        void* empty = SDL_calloc(1, 1);
        if (!empty) {
            C4_FatalError(C4_ErrorCode_OutOfMemory, "Failed to allocate empty string");
        }
        return empty;
    }

    // Add length for separators (valid items - 1)
    if (validCount > 1 && separatorLength > 0) {
        totalLength += separatorLength * (validCount - 1);
    }

    char* newString = SDL_malloc(totalLength);
    if (!newString) {
        C4_FatalError(
            C4_ErrorCode_OutOfMemory,
            "Failed to allocate string join buffer of size %zu",
            totalLength
        );
    }

    char* pointer = newString;
    bool hasAddedFirst = false;

    for (size_t i = 0; i < count; i++) {
        if (!strings[i]) {
            continue;
        }

        // Only add separator if this isn't the first string added
        if (hasAddedFirst && separatorLength > 0) {
            memcpy(pointer, separator, separatorLength);
            pointer += separatorLength;
        }

        size_t len = strlen(strings[i]);
        memcpy(pointer, strings[i], len);
        pointer += len;
        
        hasAddedFirst = true;
    }

    *pointer = '\0';
    return newString;
}