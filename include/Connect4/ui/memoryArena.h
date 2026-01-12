#pragma once
#include <stddef.h>

typedef struct {
    unsigned char* memory;
    size_t capacity;
    size_t offset;
    size_t highestRecordedOffset;
} C4_MemoryArena;

void* C4_Arena_Alloc(C4_MemoryArena* arena, size_t size);
void C4_Arena_SDL_Log_HighestOffset(C4_MemoryArena* arena);