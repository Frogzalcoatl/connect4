#include "Connect4/ui/memoryArena.h"
#include <stdlib.h>
#include <string.h>
#include "SDL3/SDL.h"

// CPU pointer size (8 bytes on 64-bit). 
// Accessing memory at non-multiple addresses can cause CPU issues.
#define C4_PTR_SIZE sizeof(void*)
 
// This is good for SIMD operations.
// Those are smth fancy about using special hardware stuf like "SSE" and "AVX"
// I dont think thats something im gonna use but ai suggested this line to be compatible with that so might as well
#define C4_ALIGNMENT (C4_PTR_SIZE * 2)

// Bitmask used to clear the lower bits of an address.
// For 16-byte alignment, this is ...11110000.
#define C4_ALIGN_MASK (C4_ALIGNMENT - 1)

// ceils x to the next multiple of C4_ALIGNMENT.
#define C4_ALIGN_UP(x) (((x) + C4_ALIGN_MASK) & ~C4_ALIGN_MASK)

static void C4_Arena_UpdateHighestRecordedOffset(C4_MemoryArena* arena) {
    if (!arena) {
        SDL_Log("Unable to update highest recorded arena offset. Arena is NULL");
        return;
    }

    if (arena->offset > arena->highestRecordedOffset) {
        arena->highestRecordedOffset = arena->offset;
    }
}

void* C4_Arena_Alloc(C4_MemoryArena* arena, size_t size) {
    size_t alignedSize = C4_ALIGN_UP(size);

    if (arena->offset + alignedSize > arena->capacity) {
        SDL_Log("Arena out of memory");
        return NULL;
    }

    void* ptr = arena->memory + arena->offset;

    arena->offset += alignedSize;

    memset(ptr, 0, alignedSize);

    C4_Arena_UpdateHighestRecordedOffset(arena);

    return ptr;
}

void C4_Arena_SDL_Log_HighestOffset(C4_MemoryArena* arena) {
    if (!arena) {
        SDL_Log("Unable to log arena's highest recorded offset. Arena is NULL");
        return;
    }
    float MB = 1024.f * 1024.f;
    float highestRecordedOffsetMB = arena->highestRecordedOffset / MB;
    float capacityMB = arena->capacity / MB;

    if (highestRecordedOffsetMB == 0.f) {
        SDL_Log("Highest Arena Offset: 0MB / %.3fMB", capacityMB); 
    } else if (highestRecordedOffsetMB < 0.001f) {
        SDL_Log("Highest Arena Offset: < 0.001MB / %.3fMB", capacityMB);   
    } else {
        SDL_Log("Highest Arena Offset: %.3fMB / %.3fMB", highestRecordedOffsetMB, capacityMB); 
    }
}