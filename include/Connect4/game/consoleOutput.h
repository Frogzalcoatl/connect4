#pragma once
#include <stdio.h>

#if defined(__GNUC__) || defined(__clang__)
    #define C4_PRINTF_CHECK(fmt_idx, args_idx) __attribute__((format(printf, fmt_idx, args_idx)))
#else
    #define C4_PRINTF_CHECK(fmt_idx, args_idx)
#endif

void C4_Log(const char* format, ...) C4_PRINTF_CHECK(1, 2);
void C4_Warn(SDL_LogCategory category, const char* format, ...) C4_PRINTF_CHECK(2, 3);

typedef enum C4_ErrorCode {
    C4_ErrorCode_None,
    C4_ErrorCode_DependencyErrorSDL,
    C4_ErrorCode_DependencyErrorTTF,
    C4_ErrorCode_DependencyErrorMIX,
    C4_ErrorCode_DatFileMissing,
    C4_ErrorCode_DatFileInvalid,
    C4_ErrorCode_OutOfMemory,
    C4_ErrorCode_RipConnect4Struct,
    C4_ErrorCode_GenericRuntimeError,
    C4_ErrorCode_Count
} C4_ErrorCode;

void C4_FatalError(C4_ErrorCode code, const char* format, ...) C4_PRINTF_CHECK(2, 3);