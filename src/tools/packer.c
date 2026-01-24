#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Connect4/tools/packerTypes.h"
#include "Connect4/tools/stringHash.h"

// Would NOT have been able to write this packer code without ai ngl. Thats why i wrote comments all over the place here

// Helper: Cross-platform string duplication
// Standard C doesn't guarantee strdup exists on all compilers (like MSVC)
char* myStrdup(const char* src) {
    size_t len = strlen(src) + 1;
    char* dest = malloc(len);
    if (dest) memcpy(dest, src, len);
    return dest;
}

typedef struct {
    C4_PackEntry entry;
    char* path;
} PackerTask;

int comparePackerTasks(const void* a, const void* b) {
    const PackerTask* taskA = (const PackerTask*)a;
    const PackerTask* taskB = (const PackerTask*)b;
    if (taskA->entry.id < taskB->entry.id) return -1;
    if (taskA->entry.id > taskB->entry.id) return 1;
    return 0;
}

int main(int argc, char* argv[]) {
    // Usage ex: ./packer assets_list.txt game.dat 1.0
    if (argc < 4) {
        printf("Usage: packer <list_file> <output_file> <header_version>\n");
        return 1;
    }

    float versionArg;
    sscanf(argv[3], "%f", &versionArg);
    printf("Header version set to %f\n", versionArg);

    // "r" stands for "Read" (Text Mode)
    FILE* listFile = fopen(argv[1], "r");
    if (!listFile) {
        printf("Error: Could not open list file %s\n", argv[1]);
        return 1;
    }

    char** filePaths = NULL;
    int fileCount = 0;
    int capacity = 0;
    
    char lineBuffer[512];

    while (fgets(lineBuffer, sizeof(lineBuffer), listFile)) {
        // Trim newline characters
        lineBuffer[strcspn(lineBuffer, "\r\n")] = 0;

        // Skip empty lines
        if (strlen(lineBuffer) == 0) {
            continue;
        }

        if (fileCount == capacity) {
            int newCapacity = capacity == 0 ? 16 : capacity * 2;
            char** newPaths = realloc(filePaths, newCapacity * sizeof(char*));

            if (!newPaths) {
                printf("Error: Out of memory expanding file list.\n");
                return 1;
            }

            filePaths = newPaths;
            capacity = newCapacity;
        }

        filePaths[fileCount] = myStrdup(lineBuffer);
        fileCount++;
    }
    fclose(listFile);

    printf("Found %d files to pack.\n", fileCount);

    C4_PackHeader header = {0};
    header.magic = C4_PACK_MAGIC;
    header.version = versionArg;
    header.fileCount = (uint32_t)fileCount;

    PackerTask* tasks = calloc(fileCount, sizeof(PackerTask));
    for (int i = 0; i < fileCount; i++) {
        tasks[i].path = filePaths[i];
        tasks[i].entry.id = stringHash(filePaths[i]);
    }

    // Sort by ID first
    qsort(tasks, fileCount, sizeof(PackerTask), comparePackerTasks);

    // Calculate offsets now that they are in order
    uint64_t currentOffset = sizeof(C4_PackHeader) + (fileCount * sizeof(C4_PackEntry));
    for (int i = 0; i < fileCount; i++) {
        FILE* asset = fopen(tasks[i].path, "rb");
        fseek(asset, 0, SEEK_END);
        tasks[i].entry.size = ftell(asset);
        tasks[i].entry.offset = currentOffset;
        currentOffset += tasks[i].entry.size;
        fclose(asset);
    }

    // Write everything
    FILE* outFile = fopen(argv[2], "wb");
    fwrite(&header, sizeof(C4_PackHeader), 1, outFile);
    
    // Write the sorted entries (the table of contents)
    for (int i = 0; i < fileCount; i++) {
        fwrite(&tasks[i].entry, sizeof(C4_PackEntry), 1, outFile);
    }

    // Write the actual file data
    for (int i = 0; i < fileCount; i++) {
        FILE* asset = fopen(tasks[i].path, "rb");
        uint8_t copyBuffer[4096 * 64];
        size_t bytesLeft = (size_t)tasks[i].entry.size;
        while (bytesLeft > 0) {
            size_t toRead = (bytesLeft < sizeof(copyBuffer)) ? bytesLeft : sizeof(copyBuffer);
            fread(copyBuffer, 1, toRead, asset);
            fwrite(copyBuffer, 1, toRead, outFile);
            bytesLeft -= toRead;
        }
        fclose(asset);
    }
    fclose(outFile);
    free(tasks);

    for (int i = 0; i < fileCount; i++) {
        free(filePaths[i]);
    }
    free(filePaths);

    printf("Successfully packed %s\n", argv[2]);
}