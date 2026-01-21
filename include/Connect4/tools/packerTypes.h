#pragma once
#include <stdint.h>

// Just 4 bytes that spell "C4PK" (Connect 4 PacK).
// A "Magic" number
#define C4_PACK_MAGIC 0x4B503443

#pragma pack(push, 1) // Force 1-byte alignment (no padding)
// Uses 32 bit integers instead of 64 bc 64 would be a waste of space.
// No way im gonna have more than 4 billion individual asset files.
typedef struct C4_PackHeader {
    uint32_t magic;
    float version;
    uint32_t fileCount;
    // The underscore marks "Internal use only / dont touch" like xsku does.
    // Honestly might implement the underscore in my other structs too
    uint32_t _pad;
} C4_PackHeader;

// uint64 to support large file sizes.
typedef struct C4_PackEntry {
    uint64_t id;
    uint64_t offset;
    uint64_t size;
} C4_PackEntry;
#pragma pack(pop)