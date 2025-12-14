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
    uint8_t amountToWin;
    C4_SlotState* cells;
    C4_SlotState currentPlayer;
    C4_SlotState* cellCheckBuffer;
    size_t cellCheckBufferSize;
    size_t cellCheckCount;
} C4_Board;

typedef enum {
    C4_Board_RowAxis_NorthSouth,
    C4_Board_RowAxis_EastWest,
    C4_Board_RowAxis_NorthEastSouthWest,
    C4_Board_RowAxis_NorthWestSouthEast

} C4_Board_RowAxis;

C4_Board* C4_Board_Create(uint8_t width, uint8_t height, uint8_t amountToWin);
void C4_Board_Destroy(C4_Board* board);
C4_SlotState C4_Board_GetSlot(C4_Board* board, uint8_t x, uint8_t y);
bool C4_Board_SetSlot(C4_Board* board, uint8_t x, uint8_t y, C4_SlotState state);
int64_t C4_Board_DoMove(C4_Board* board, uint8_t inColumn);
void C4_Board_UpdateTestStr(C4_Board* board, char* buffer, size_t bufferSize);
C4_SlotState C4_Board_GetWinner(C4_Board* board, size_t mostRecentMoveIndex);