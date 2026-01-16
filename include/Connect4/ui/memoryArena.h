#pragma once
#include <stddef.h>

// Disable the "zero-sized array" warning for MSVC
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4200)
#endif

typedef struct C4_ArenaBlock {
    struct C4_ArenaBlock* next;
    size_t capacity;
    size_t offset;
    // Flexible array properties must be the very last element of the struct
    unsigned char memory[];
} C4_ArenaBlock;

// Re-enable the warning for the rest of the code
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

typedef struct C4_MemoryArena {
    C4_ArenaBlock* firstBlock;
    C4_ArenaBlock* currentBlock;
    size_t defaultBlockSize;
} C4_MemoryArena;

void C4_Arena_Init(C4_MemoryArena* arena, size_t blockSize);
void* C4_Arena_Alloc(C4_MemoryArena* arena, size_t size);
void C4_Arena_SDL_LogMemory(C4_MemoryArena* arena);
void C4_Arena_Destroy(C4_MemoryArena* arena);

typedef struct C4_ArenaTemp {
    C4_MemoryArena* arena;
    C4_ArenaBlock* block;
    size_t offset;
} C4_ArenaTemp;

C4_ArenaTemp C4_Arena_BeginTemp(C4_MemoryArena* arena);
void C4_Arena_EndTemp(C4_ArenaTemp* temp);