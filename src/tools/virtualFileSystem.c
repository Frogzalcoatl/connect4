#include "Connect4/tools/virtualFileSystem.h"
#include "Connect4/tools/packerTypes.h"
#include "Connect4/tools/stringHash.h"
#include "Connect4/game/events.h"
#include "Connect4/dat.h"
#include "Connect4/game/consoleOutput.h"
#include "SDL3/SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Using a .dat file makes it significantly easier to read files on android
// Dont have to go through jni nightmare again

typedef struct C4_VFS_State {
    SDL_IOStream* packFile;
    // For thread safety
    SDL_Mutex* mutex;
    
    C4_PackEntry* entries;
    int fileCount;
    uint64_t dataStartOffset;
    const char* basePath;
    bool useFolder;
    char datPath[2048];
} C4_VFS_State;

static C4_VFS_State vfs = {0};
static void C4_VFS_InitThroughFolder(void) {
    char assetsFolderPath[2048];

    SDL_snprintf(assetsFolderPath, sizeof(assetsFolderPath), "%s%s", vfs.basePath, "/assets");

    SDL_PathInfo pathInfo;

    bool getPathResult = SDL_GetPathInfo(assetsFolderPath, &pathInfo);

    if (getPathResult && pathInfo.type == SDL_PATHTYPE_DIRECTORY) {
        vfs.useFolder = true;
        C4_Log("Initialized VFS using assets folder");
        return;
    }

    vfs.useFolder = false;
}

static void C4_VFS_InitThroughDatFile(void) {
    vfs.packFile = SDL_IOFromFile(vfs.datPath, "rb");
    if (!vfs.packFile) {
        if (vfs.useFolder) {
            return;
        }
        C4_FatalError(C4_ErrorCode_DatFileMissing, " ");
    }

    C4_PackHeader header;
    SDL_ReadIO(vfs.packFile, &header, sizeof(C4_PackHeader));

    if (header.magic != C4_PACK_MAGIC) {
        if (vfs.useFolder) {
            return;
        }
        C4_FatalError(C4_ErrorCode_DatFileInvalid, " ");
    }

    if (header.version != GAME_VERSION) {
        if (vfs.useFolder) {
            return;
        }
        C4_FatalError(
            C4_ErrorCode_DatFileInvalid, 
            "Expected version: %.1f\nFound version: %.1f", 
            GAME_VERSION, 
            header.version
        );
    }

    // Allocate memory only for the directory entries (table of contents)
    vfs.fileCount = header.fileCount;
    size_t entriesSize = vfs.fileCount * sizeof(C4_PackEntry);

    vfs.entries = SDL_malloc(entriesSize);
    if (!vfs.entries) {
        C4_FatalError(C4_ErrorCode_OutOfMemory, "Failed to allocate VFS Table of Contents");
    }

    // Read the entries into RAM
    SDL_ReadIO(vfs.packFile, vfs.entries, entriesSize);

    C4_Log("Initialized VFS using %s", vfs.datPath);
}

void C4_VFS_Init(const char* filePath) {
    assert(filePath);

    vfs.basePath = SDL_GetBasePath();
    SDL_strlcpy(vfs.datPath, filePath, sizeof(vfs.datPath));

    C4_VFS_InitThroughFolder();

    C4_VFS_InitThroughDatFile();
}

void C4_VFS_Quit(void) {
    if (vfs.packFile) {
        SDL_CloseIO(vfs.packFile);
        vfs.packFile = NULL;
    }
    if (vfs.mutex) {
        SDL_DestroyMutex(vfs.mutex);
        vfs.mutex = NULL;
    }
    if (vfs.entries) {
        SDL_free(vfs.entries);
        vfs.entries = NULL;
    }
    C4_Log("Quit VFS");
}

static void* C4_VFS_ReadFileFromFolder(const char* filename, size_t* outSize) {
    assert(filename);

    char fullFilePath[2048];
    SDL_snprintf(fullFilePath, sizeof(fullFilePath), "%s%s%s", vfs.basePath, "/", filename);

    // Using read binary "rb". "r" can corrupt binary data by translating line endings \n to \r\n
    SDL_IOStream* io = SDL_IOFromFile(fullFilePath, "rb");
    if (!io) {
        return NULL;
    }

    Sint64 ioSize = SDL_GetIOSize(io);

    if (ioSize < 0) {
        SDL_CloseIO(io);
        return NULL;
    }

    void* data = SDL_malloc(ioSize);
    if (!data) {
        C4_FatalError(C4_ErrorCode_OutOfMemory, " ");
    }

    SDL_ReadIO(io, data, ioSize);
    SDL_CloseIO(io);

    if (outSize) *outSize = ioSize;

    return data;
}

void* C4_VFS_ReadFile(const char* filename, size_t* outSize) {
    assert(filename);

    if (vfs.useFolder) {
        void* result = C4_VFS_ReadFileFromFolder(filename, outSize);
        if (result) {
            return result;
        }
    }

    if (!vfs.packFile) {
        C4_VFS_InitThroughDatFile();
    }

    if (!vfs.packFile) {
        C4_Warn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Unable to read file %s",
            filename
        );
        return NULL;
    }

    uint64_t id = stringHash(filename);
    C4_PackEntry* entry = NULL;

    if (!vfs.entries) {
        C4_Warn(SDL_LOG_CATEGORY_APPLICATION, "Unable to access .dat file entries. Attempting to reinitialize dat file");
        if (!vfs.entries) {
            return NULL;
        }
    }
    
    for (int i = 0; i < vfs.fileCount; i++) {
        if (vfs.entries[i].id == id) {
            entry = &vfs.entries[i];
            break;
        }
    }

    if (!entry) {
        C4_Warn(SDL_LOG_CATEGORY_APPLICATION, "Unable to read file in %s:\n%s", MASTER_DAT_PATH, filename);
        return NULL;
    }

    void* buffer = SDL_malloc(entry->size);
    if (!buffer) {
        C4_FatalError(C4_ErrorCode_OutOfMemory, "Failed to allocate buffer for file: %s", filename);
    }
    
    SDL_SeekIO(vfs.packFile, entry->offset, SDL_IO_SEEK_SET);
    
    size_t bytesRead = SDL_ReadIO(vfs.packFile, buffer, entry->size);
    if (bytesRead != entry->size) {
        SDL_free(buffer);
        C4_FatalError(C4_ErrorCode_DatFileInvalid, "Unable to read full data for asset: %s", filename);
    }
    
    if (outSize) {
        *outSize = bytesRead;
    }
    return buffer;
}

void C4_VFS_FreeFile(void* buffer) {
    if (buffer) {
        SDL_free(buffer);
    }
}