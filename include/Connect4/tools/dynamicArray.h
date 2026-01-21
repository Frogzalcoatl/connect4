#pragma once
#include "Connect4/game/consoleOutput.h"
#include "SDL3/SDL.h"

// This macro expects "arr" to be a struct with properties: count, capacity, and data (a pointer to the start of the array).
// Do while ensures all if else syntax works properly
#define C4_DynamicArray_Push_Back(arr, value) \
    do { \
        if ((arr).count >= (arr).capacity) { \
            size_t newCapacity = (arr).capacity == 0 ? 4 : (arr).capacity * 2; \
            void* newPointer = SDL_realloc((arr).data, newCapacity * sizeof(*(arr).data)); \
            if (!newPointer) { \
                C4_FatalError(C4_ErrorCode_OutOfMemory, "Dynamic array growth failed"); \
            } \
            (arr).data = newPointer; \
            (arr).capacity = newCapacity; \
        } \
        (arr).data[(arr).count++] = (value); \
    } while(0)

#define C4_DynamicArray_Pop(arr) \
    do { \
        if ((arr).count > 0) (arr).count--; \
    } while(0)

#define C4_DynamicArray_Free(arr) \
    do { \
        if ((arr).data) { \
            SDL_free((arr).data); \
            (arr).data = NULL; \
        } \
        (arr).count = 0; \
        (arr).capacity = 0; \
    } while(0)

// Maintains the order of the array
#define C4_DynamicArray_Remove_Shift(arr, index) \
    do { \
        if ((index) >= 0 && (index) < (arr).count) { \
            int elementsToMove = (arr).count - 1 - (index); \
            if (elementsToMove > 0) { \
                SDL_memmove( \
                    &(arr).data[index], \
                    &(arr).data[index + 1], \
                    elementsToMove * sizeof(*(arr).data) \
                ); \
            } \
            (arr).count--; \
        } \
    } while(0)

// Overwrites the removed index with last element of array (faster than shift)
#define C4_DynamicArray_Remove_ReplaceWithBack(arr, index) \
    do { \
        if ((index) >= 0 && (index) < (arr).count) { \
            (arr).data[index] = (arr).data[(arr).count - 1]; \
            (arr).count--; \
        } \
    } while(0)

/*
Prfeixing emptyIndexFound and i with underscores so they
dont conflict with variables in functions where this macro is used.
*/
// Requires array elements to be nullable pointers
#define C4_DynamicArray_Push_Ptr_FillHole(arr, value) \
    do { \
        bool _emptyIndexFound = false; \
        for (size_t _i = 0; _i < (arr).count; _i++) { \
            if (!(arr).data[_i]) { \
                (arr).data[_i] = value; \
                _emptyIndexFound = true; \
                break; \
            } \
        } \
        if (!_emptyIndexFound) { \
            C4_DynamicArray_Push_Back(arr, value); \
        } \
    } while(0)

// Requires array elements to be nullable pointers
#define C4_DynamicArray_Remove_Ptr_MakeHole(arr, index) \
    do { \
        if ((index) >= 0 && (index) < (arr).count) { \
            (arr).data[index] = NULL; \
        } \
    } while(0)