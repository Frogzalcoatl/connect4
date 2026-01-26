#include "SDL3/SDL.h"
#include "Connect4/game/consoleOutput.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

void GetCurrentTimeStrings(char* buffer, size_t bufferSize) {
    SDL_Time ticks;
    SDL_DateTime dt;

    if (SDL_GetCurrentTime(&ticks) && SDL_TimeToDateTime(ticks, &dt, true)) {
        SDL_snprintf(buffer, bufferSize, "%02d:%02d:%02d", dt.hour, dt.minute, dt.second);
    } else {
        SDL_snprintf(buffer, bufferSize, "00:00:00");
    }
}

static void C4_LogOutputFunction(void *userdata, int category, SDL_LogPriority priority, const char *message) {
    (void)userdata;
    
    char timeStr[32];
    GetCurrentTimeStrings(timeStr, sizeof(timeStr));

    const char* priorityStr;
    switch (priority) {
        case SDL_LOG_PRIORITY_WARN:  priorityStr = "WARN";  break;
        case SDL_LOG_PRIORITY_ERROR: priorityStr = "ERROR"; break;
        default:                     priorityStr = "INFO";  break;
    }

    const char* catStr;
    switch (category) {
        case SDL_LOG_CATEGORY_SYSTEM: catStr = "SYSTEM"; break;
        case SDL_LOG_CATEGORY_AUDIO: catStr = "AUDIO"; break;
        case SDL_LOG_CATEGORY_INPUT: catStr = "INPUT"; break;
        default: catStr = "APP"; break;
    }

    fprintf(stdout, "[%s] [%s/%s]: %s\n", timeStr, catStr, priorityStr, message);
    fflush(stdout);
}

void C4_InitLogs(void) {
    SDL_SetLogOutputFunction(C4_LogOutputFunction, NULL);
}

void C4_Log(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // SDL_LogMessageV sends the log through the output function above
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, format, args);
    va_end(args);
}

void C4_Warn(SDL_LogCategory category, const char* format, ...) {
    va_list args;
    va_start(args, format);
    SDL_LogMessageV(category, SDL_LOG_PRIORITY_WARN, format, args);
    va_end(args);
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
    "Unable to initialize internal game structure."
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