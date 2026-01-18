#include "Connect4/tools/virtualFileSystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Connect4/tools/packerTypes.h"
#include "Connect4/tools/stringHash.h"
#include "SDL3/SDL.h"

typedef struct C4_VFS_State {
    SDL_IOStream* packFile;
    // For thread safety
    SDL_Mutex* mutex;
    
    C4_PackEntry* entries;
    int fileCount;
    uint64_t dataStartOffset;
} C4_VFS_State;

static C4_VFS_State vfs;

bool C4_VFS_Init(const char* filePath) {
    vfs.packFile = SDL_IOFromFile(filePath, "rb");
    if (!vfs.packFile) {
        return false;
    }

    C4_PackHeader header;
    SDL_ReadIO(vfs.packFile, &header, sizeof(C4_PackHeader));

    if (header.magic != C4_PACK_MAGIC) {
        return false;
    }

    // Allocate memory only for the directory entries (table of contents)
    vfs.fileCount = header.fileCount;
    size_t entriesSize = vfs.fileCount * sizeof(C4_PackEntry);

    vfs.entries = SDL_malloc(entriesSize);

    // Read the entries into RAM
    SDL_ReadIO(vfs.packFile, vfs.entries, entriesSize);

    return true;
}

void C4_VFS_Quit(void) {
    if (vfs.packFile) {
        SDL_CloseIO(vfs.packFile);
    }
    if (vfs.mutex) {
        SDL_DestroyMutex(vfs.mutex);
    }
    if (vfs.entries) {
        SDL_free(vfs.entries);
    }
}

void* C4_VFS_ReadFile(const char* filename, size_t* outSize) {
    uint64_t id = stringHash(filename);
    C4_PackEntry* entry = NULL;
    
    for (int i = 0; i < vfs.fileCount; i++) {
        if (vfs.entries[i].id == id) {
            entry = &vfs.entries[i];
            break;
        }
    }

    if (!entry) {
        return NULL;
    }

    void* buffer = SDL_malloc(entry->size);
    
    SDL_SeekIO(vfs.packFile, entry->offset, SDL_IO_SEEK_SET);
    
    size_t bytesRead = SDL_ReadIO(vfs.packFile, buffer, entry->size);
    
    if (outSize) {
        *outSize = bytesRead;
    }
    return buffer;
}

void C4_VFS_FreeFile(void* buffer) {
    SDL_free(buffer);
}