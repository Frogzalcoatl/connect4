#include "Connect4/tools/stringHash.h"

// This turns a string like "assets/sounds/jump.ogg" into a unique number.
// Using numbers because comparing numbers is faster than comparing strings.
// Uses the FNV-1a (Fowler-Noll-Vo) algorithm
uint64_t stringHash(const char* str) {
    // The FNV offset basis. A starting point so the hash doesnt start at 0.
    // Minimizes collisions (where two different strings accidentally result in the same number)
    // A 50% chance of a collision would require over 5 billion files lol
    uint64_t hash = 14695981039346656037ULL;
    while (*str) {
        // The bitwise exclusive or assignment operator
        hash ^= (unsigned char)*str++;
        // The FNV prime
        hash *= 1099511628211ULL;
    }
    return hash;
}