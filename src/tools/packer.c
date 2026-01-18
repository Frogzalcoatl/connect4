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

int main(int argc, char* argv[]) {
    // Usage ex: ./packer assets_list.txt game.dat
    if (argc < 3) {
        printf("Usage: packer <list_file> <output_file>\n");
    }

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
    header.version = 1;
    header.fileCount = (uint32_t)fileCount;

    C4_PackEntry* entries = calloc(fileCount, sizeof(C4_PackEntry));
    if (!entries && fileCount > 0) {
        printf("Error: Out of memory for entries.\n");
        return 1;
    }

    uint64_t currentOffset = sizeof(C4_PackHeader) + (fileCount * sizeof(C4_PackEntry));

    for (int i = 0; i < fileCount; i++) {
        const char* path = filePaths[i];

        // "rb" stands for read binary
        FILE* asset = fopen(path, "rb");
        if (!asset) {
            printf("Error: Could not find asset: %s\n", path);
            free(entries);
            for (int k = 0; k < fileCount; k++) {
                free(filePaths[k]);
            }
            free(filePaths);
            return 1;
        }

        fseek(asset, 0, SEEK_END);
        uint64_t size = ftell(asset);
        fclose(asset);

        entries[i].id = stringHash(path);
        entries[i].size = size;
        entries[i].offset = currentOffset;

        printf("Packing %s (ID: %zu, Size %zu)\n", path, entries[i].id, size);
        currentOffset += size;
    }

    // Write output
    FILE* outFile = fopen(argv[2], "wb");
    if (!outFile) {
        printf("Error: Could not create output file %s\n", argv[2]);
        return 1;
    }

    // Just writing the position of all the entires. Sort of liek a table of contents
    fwrite(&header, sizeof(C4_PackHeader), 1, outFile);
    fwrite(entries, sizeof(C4_PackEntry), fileCount, outFile);
    
    // Write blobs
    for (int i = 0; i < fileCount; i++) {
        FILE* asset = fopen(filePaths[i], "rb");

        // Optimization: read/write in chunks instead of allocating massive buffers
        uint8_t copyBuffer[4096* 64];
        size_t bytesLeft = (size_t)entries[i].size;

        while (bytesLeft > 0) {
            size_t toRead = (bytesLeft < sizeof(copyBuffer)) ? bytesLeft : sizeof(copyBuffer);
            fread(copyBuffer, 1, toRead, asset);
            fwrite(copyBuffer, 1, toRead, outFile);
            bytesLeft -= toRead;
        }

        fclose(asset);
    }

    fclose(outFile);

    free(entries);
    for (int i = 0; i < fileCount; i++) {
        free(filePaths[i]);
    }
    free(filePaths);

    printf("Successfully packed %s\n", argv[2]);
}