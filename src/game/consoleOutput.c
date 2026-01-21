#include "SDL3/SDL.h"
#include "Connect4/game/consoleOutput.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

void C4_Log(const char* format, ...) {

    char text[2048] = "";

    if (format && *format != '\0') {
        va_list args;
        va_start(args, format);
        vsnprintf(text, sizeof(text), format, args);
        va_end(args);
    }

    SDL_Log("[INFO] %s", text);
}

void C4_Warn(SDL_LogCategory category, const char* format, ...) {

    char text[2048] = "";

    if (format && *format != '\0') {
        va_list args;
        va_start(args, format);
        vsnprintf(text, sizeof(text), format, args);
        va_end(args);
    }

    SDL_LogWarn(category, "[WARN] %s", text);
}

static const char* ERROR_MESSAGES[C4_ErrorCode_Count] = {
    "No error occurred (Wait, why did the game crash then?)",
    "Error occurred within SDL.",
    "Error occurred within SDL_ttf.",
    "Error occurred within SDL_mixer.",
    "Connect4.dat not found.",
    "Connect4.dat is invalid. Please try redownloading.",
    "System Out of Memory.",
    "Runtime error.",
    "Unable to create game structure."
};

void C4_FatalError(C4_ErrorCode code, const char* format, ...) {
    if (code < 0 || code >= C4_ErrorCode_Count) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid error code thrown: %d", code);
        exit(EXIT_FAILURE);
    }

    char details[1024] = "";
    
    if (format && *format != '\0') {
        va_list args;
        va_start(args, format);
        vsnprintf(details, sizeof(details), format, args);
        va_end(args);
    }

    char finalMessage[2048];
    const char* baseMessage = ERROR_MESSAGES[code];

    if (format) {
        snprintf(finalMessage, sizeof(finalMessage), "%s\n%s", baseMessage, details);
    } else {
        snprintf(finalMessage, sizeof(finalMessage), "%s", baseMessage);
    }

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", finalMessage);

    SDL_ShowSimpleMessageBox(
        SDL_MESSAGEBOX_ERROR,
        "Connect4 Fatal Error",
        finalMessage,
        NULL
    );

    exit(EXIT_FAILURE);
}