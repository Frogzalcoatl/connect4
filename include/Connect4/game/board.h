#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    C4_SlotState_Empty,
    C4_SlotState_Player1,
    C4_SlotState_Player2
} C4_SlotState;

typedef struct {
    uint8_t width;
    uint8_t height;
    C4_SlotState* cells;
    C4_SlotState currentPlayer;
} C4_Board;

C4_Board* C4_Board_Create(uint8_t width, uint8_t height);
void C4_Board_Destroy(C4_Board* board);
C4_SlotState C4_Board_GetSlot(C4_Board* board, uint8_t x, uint8_t y);
bool C4_Board_SetSlot(C4_Board* board, uint8_t x, uint8_t y, C4_SlotState state);
// Returns true if valid move was made
bool C4_Board_DoMove(C4_Board* board, uint8_t inColumn);
void C4_Board_UpdateTestStr(C4_Board* board, char* buffer, size_t bufferSize);
C4_SlotState C4_Board_GetWinner(C4_Board* board, size_t mostRecentMoveIndex);