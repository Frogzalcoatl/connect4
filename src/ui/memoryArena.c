#include "Connect4/ui/memoryArena.h"
#include <stdlib.h>
#include <string.h>
#include "SDL3/SDL.h"
#include "Connect4/ui/utils.h"

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

void C4_Arena_Init(C4_MemoryArena* arena, size_t blockSize) {
    arena->firstBlock = NULL;
    arena->currentBlock = NULL;
    arena->defaultBlockSize = blockSize;
}

static C4_ArenaBlock* C4_Arena_CreateBlock(size_t size) {
    C4_ArenaBlock* block = malloc(sizeof(C4_ArenaBlock) + size);
    block->next = NULL;
    block->capacity = size;
    block->offset = 0;
    return block;
}

void* C4_Arena_Alloc(C4_MemoryArena* arena, size_t size) {
    size_t alignedSize = C4_ALIGN_UP(size);

    if (
        arena->currentBlock == NULL || 
       (arena->currentBlock->offset + alignedSize) > arena->currentBlock->capacity
    ) {
        size_t newSize = C4_ULLMax(arena->defaultBlockSize, alignedSize);
        
        C4_ArenaBlock* newBlock = C4_Arena_CreateBlock(newSize);
        
        if (arena->currentBlock) {
            arena->currentBlock->next = newBlock;
        } else {
            arena->firstBlock = newBlock;
        }
        arena->currentBlock = newBlock;
    }

    void* ptr = arena->currentBlock->memory + arena->currentBlock->offset;
    arena->currentBlock->offset += alignedSize;
    
    // Clear memory to zero
    memset(ptr, 0, alignedSize);
    
    return ptr;
}

void C4_Arena_SDL_LogMemory(C4_MemoryArena* arena) {
    if (!arena) {
        SDL_Log("Unable to log arena memory. Arena is NULL");
        return;
    }

    size_t totalBytes = 0;
    size_t totalCapacity = 0;
    C4_ArenaBlock* current = arena->firstBlock;

    while (current) {
        totalBytes += current->offset;
        totalCapacity += current->capacity;
        current = current->next;
    }

    float MB = 1024.f * 1024.f;

    double totalMB = totalBytes / MB;
    double totalCapacityMB = totalCapacity / MB;

    if (totalMB == 0.f) {
        SDL_Log("Arena Memory: 0MB / %.3fMB", totalCapacityMB); 
    } else if (totalMB < 0.001f) {
        SDL_Log("Arena Memory: < 0.001MB / %.3fMB", totalCapacityMB);   
    } else {
        SDL_Log("Arena Memory: %.3fMB / %.3fMB", totalMB, totalCapacityMB); 
    }
}

void C4_Arena_Destroy(C4_MemoryArena* arena) {
    if (!arena) {
        SDL_Log("Unable to destroy arena. Pointer is NULL");
        return;
    }
    C4_ArenaBlock* current = arena->firstBlock;
    while (current) {
        C4_ArenaBlock* next = current->next;
        free(current);
        current = next;
    }
    arena->firstBlock = NULL;
    arena->currentBlock = NULL;
}

C4_ArenaTemp C4_Arena_BeginTemp(C4_MemoryArena* arena) {
    if (!arena) {
        SDL_Log("Unable to begin arena temp. Arena is NULL. Returning dummy temp");
        return (C4_ArenaTemp){
            .arena = NULL,
            .block = NULL,
            .offset = 0
        };
    }
    return (C4_ArenaTemp){
        .arena = arena,
        .block = arena->currentBlock,
        .offset = (arena->currentBlock ? arena->currentBlock->offset : 0)
    };
}

void C4_Arena_EndTemp(C4_ArenaTemp* temp) {
    if (!temp) {
        SDL_Log("Unable to clear arena temp. Temp is NULL");
        return;
    }
    temp->arena->currentBlock = temp->block;
    
    if (temp->arena->currentBlock) {
        temp->arena->currentBlock->offset = temp->offset;
    }
}