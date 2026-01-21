#include "SDL3/SDL.h"
#include "Connect4/game/board.h"
#include "Connect4/ui/utils.h"
#include "Connect4/game/consoleOutput.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void C4_Board_SetSize(C4_Board* board, uint8_t width, uint8_t height) {
    assert(board);
    assert(width >= 1);
    assert(height >= 1);

    C4_SlotState* newCells = SDL_calloc(width * height, sizeof(C4_SlotState));
    if (!newCells) {
        C4_FatalError(
            C4_ErrorCode_OutOfMemory,
            "Unable to set board size. Unable to allocate memory for connect4 board cells"
        );
    }

    size_t newBufferSize = (size_t)C4_Max((int)width, (int)height);
    C4_SlotState* newCellCheckBuffer = SDL_calloc(newBufferSize, sizeof(C4_SlotState));
    if (!newCellCheckBuffer) {
        C4_FatalError(
            C4_ErrorCode_OutOfMemory,
            "Unable to set board size. Unable to allocate memory for connect4 board cell check buffer"
        );
    }

    SDL_free(board->cells);
    SDL_free(board->cellCheckBuffer);

    board->cells = newCells;
    board->cellCheckBufferSize = newBufferSize;
    board->cellCheckBuffer = newCellCheckBuffer;
    board->width = width;
    board->height = height;
    board->currentPlayer = C4_SlotState_Player1;
}

C4_Board* C4_Board_Create(uint8_t width, uint8_t height, uint8_t amountToWin) {
    assert(width >= 1);
    assert(height >= 1);
    assert(amountToWin >= 1);

    C4_Board* newBoard = SDL_calloc(1, sizeof(C4_Board));
    C4_Board_SetSize(newBoard, width, height);

    newBoard->cellCheckCount = 0;
    newBoard->width = width;
    newBoard->height = height;
    newBoard->amountToWin = amountToWin;
    newBoard->currentPlayer = C4_SlotState_Player1;

    C4_Log("Connect4 board setup complete");

    return newBoard;
}

void C4_Board_Destroy(C4_Board* board) {
    if (!board) {
        return;
    }

    if (board->cells) {
        SDL_free(board->cells);
    }

    if (board->cellCheckBuffer) {
        SDL_free(board->cellCheckBuffer);
    }

    SDL_free(board);
}

void C4_Board_Reset(C4_Board* board) {
    assert(board);
    
    for (size_t i = 0; i < board->width * board->height; i++) {
        board->cells[i] = C4_SlotState_Empty;
    }
    board->currentPlayer = C4_SlotState_Player1;
}

C4_SlotState C4_Board_GetSlot(C4_Board* board, uint8_t x, uint8_t y) {
    assert(board);
    assert (x < board->width && y < board->height);

    return board->cells[board->width * y + x];
}

bool C4_Board_SetSlot(C4_Board* board, uint8_t x, uint8_t y, C4_SlotState state) {
    assert(board);
    assert(x < board->width);
    assert(y < board->height);
    assert(state <= C4_SlotState_Player2 && state >= C4_SlotState_Empty);

    board->cells[board->width * y + x] = state;
    return true;
}

static void C4_Board_SwapCurrentPlayer(C4_Board* board) {
    assert(board);
    
    board->currentPlayer = board->currentPlayer == C4_SlotState_Player1 ?
        C4_SlotState_Player2 : C4_SlotState_Player1;
}

// Returns index of move. -1 if move is invalid.
int C4_Board_DoMove(C4_Board* board, uint8_t inColumn) {
    assert(board);

    // If column is full returns -1.
    if (
        board->cells[inColumn] != C4_SlotState_Empty ||
        board->currentPlayer == C4_SlotState_Empty ||
        inColumn >= board->width
    ) {
        return -1;
    }
    for (size_t row = board->height - 1; row < board->height; row--) {
        if (board->cells[row * board->width + inColumn] == C4_SlotState_Empty) {
            board->cells[row * board->width + inColumn] = board->currentPlayer;
            C4_Board_SwapCurrentPlayer(board);
            return (int)row * (int)board->width + (int)inColumn;
        }
    }
    return -1;
}

char C4_Board_GetCharForState(C4_SlotState state) {
    switch (state) {
        case C4_SlotState_Player1: return 'X';
        case C4_SlotState_Player2: return 'O';
        case C4_SlotState_Empty: return '.';
        default: return '?';
    }
}
/*
static void C4_Board_PrintCellCheckBuffer(C4_Board* board) {
    if (!board) {
        C4_Log("Unable to print cell check buffer. Board is NULL");
        return;
    }
    const size_t strSize = board->cellCheckCount * 2 + 1;
    char* str = SDL_malloc(strSize);
    if (!str) {
        C4_Log("Unable to allocate memory for cellCheckBuffer string");
        return;
    }
    for (size_t i = 0; i < board->cellCheckCount; i++) {
        str[i * 2] = C4_Board_GetCharForState(board->cellCheckBuffer[i]);
        str[i * 2 + 1] = ' ';
        
    }
    str[strSize - 1] = '\0';
    C4_Log("%s", str);
    SDL_free(str);
}
*/
static void C4_Board_UpdateCellCheckBuffer(C4_Board* board, C4_Board_RowAxis axis, size_t atIndex) {
    assert(board);
    
    // Sets all values of cellCheckBuffer to zero.
    SDL_memset(board->cellCheckBuffer, 0, (size_t)C4_Max((int)board->width, (int)board->height) * sizeof(C4_SlotState));
    size_t bufferIndex = 0;
    switch (axis) {
        case C4_Board_RowAxis_NorthSouth: {
            size_t topIndex = atIndex % board->width;
            for (size_t boardIndex = topIndex; boardIndex < (board->width * board->height); boardIndex += board->width) {
                board->cellCheckBuffer[bufferIndex] = board->cells[boardIndex];
                bufferIndex++;
            }
            board->cellCheckCount = board->height;
        }; break;
        case C4_Board_RowAxis_EastWest: {
            size_t leftestIndex = atIndex - (atIndex % board->width);
            size_t rightestIndex = leftestIndex + board->width - 1;
            for (size_t boardIndex = leftestIndex; boardIndex < rightestIndex + 1; boardIndex++) {
                board->cellCheckBuffer[bufferIndex] = board->cells[boardIndex];
                bufferIndex++;
            }
            board->cellCheckCount = board->width;
        }; break;
        case C4_Board_RowAxis_NorthEastSouthWest: {
            size_t northEastestIndex = 0;
            for (size_t i = atIndex; i < (board->width * board->height); i -= (board->width - 1)) {
                northEastestIndex = i;
                size_t nextIndex = i - (board->width - 1);
                // If this is true, the diagonal row has hit the right wall, meaning the loop should end.
                if (
                    nextIndex % board->width < i % board->width ||
                    nextIndex >= (board->width * board->height)
                ) {
                    break;
                }
            }
            board->cellCheckCount = 0;
            for (size_t boardIndex = northEastestIndex; boardIndex < (board->width * board->height); boardIndex += (board->width - 1)) {
                board->cellCheckBuffer[bufferIndex] = board->cells[boardIndex];
                bufferIndex++;
                board->cellCheckCount++;
                // Since this loop increments by width - 1, if boardIndex is the bottom left index of the board,
                // nextIndex would be the bottom right corner of the board, which should not be included in the diagonal row.
                size_t nextIndex = boardIndex + (board->width - 1);
                if (
                    nextIndex >= (board->width * board->height) || 
                    nextIndex % board->width > boardIndex % board->width
                ) {
                    break;
                }
            }
        }; break;
        case C4_Board_RowAxis_NorthWestSouthEast: {
            size_t northWestestIndex = 0;
            for (size_t i = atIndex; i < (board->width * board->height); i -= (board->width + 1)) {
                northWestestIndex = i;
                size_t nextIndex = i - (board->width + 1);
                // If this is true, the diagonal row has hit the left wall, meaning the loop should end.
                if (
                    nextIndex % board->width > i % board->width ||
                    nextIndex >= (board->width * board->height)
                ) {
                    break;
                }
            }
            board->cellCheckCount = 0;
            for (size_t boardIndex = northWestestIndex; boardIndex < (board->width * board->height); boardIndex += (board->width + 1)) {
                board->cellCheckBuffer[bufferIndex] = board->cells[boardIndex];
                bufferIndex++;
                board->cellCheckCount++;
                size_t nextIndex = boardIndex + (board->width + 1);
                if (
                    nextIndex >= (board->width * board->height) || 
                    nextIndex % board->width < boardIndex % board->width
                ) {
                    break;
                }
            }
        }; break;
    }
}

static C4_SlotState C4_Board_TestCellCheckBuffer(C4_Board* board) {
    assert(board);

    C4_SlotState previousState = C4_SlotState_Empty;
    size_t amountInRow = 1;
    for (size_t i = 0; i < board->cellCheckCount; i++) {
        if (previousState != C4_SlotState_Empty && previousState == board->cellCheckBuffer[i]) {
            amountInRow++;
        } else {
            amountInRow = 1;
        }
        if (amountInRow == board->amountToWin) {
            return board->cellCheckBuffer[i];
        }
        previousState = board->cellCheckBuffer[i];
    }
    return C4_SlotState_Empty;
}

C4_SlotState C4_Board_GetWinner(C4_Board* board, size_t mostRecentMoveIndex) {
    assert(board);

    C4_SlotState bufferTestResult = C4_SlotState_Empty;
    // I only need to check the lines of the most recently placed piece rather than the whole board
    // Current plan: On every axis (S/N, E/W, NE/SW, SE/NW), check each row for amonut to win
    C4_Board_UpdateCellCheckBuffer(board, C4_Board_RowAxis_NorthSouth, mostRecentMoveIndex);
    bufferTestResult = C4_Board_TestCellCheckBuffer(board);
    if (bufferTestResult != C4_SlotState_Empty) {
        return bufferTestResult;
    }
    C4_Board_UpdateCellCheckBuffer(board, C4_Board_RowAxis_EastWest, mostRecentMoveIndex);
    bufferTestResult = C4_Board_TestCellCheckBuffer(board);
    if (bufferTestResult != C4_SlotState_Empty) {
        return bufferTestResult;
    }
    C4_Board_UpdateCellCheckBuffer(board, C4_Board_RowAxis_NorthEastSouthWest, mostRecentMoveIndex);
    bufferTestResult = C4_Board_TestCellCheckBuffer(board);
    if (bufferTestResult != C4_SlotState_Empty) {
        return bufferTestResult;
    }
    C4_Board_UpdateCellCheckBuffer(board, C4_Board_RowAxis_NorthWestSouthEast, mostRecentMoveIndex);
    bufferTestResult = C4_Board_TestCellCheckBuffer(board);
    if (bufferTestResult != C4_SlotState_Empty) {
        return bufferTestResult;
    }
    return C4_SlotState_Empty;
}

void C4_Board_UpdateTestStr(C4_Board* board, char* strToUpdate, size_t strSize) {
    assert(board);

    size_t amountNeeded = board->width * board->height + board->height + 1;
    if (amountNeeded > strSize) {
        C4_Warn(
            SDL_LOG_CATEGORY_APPLICATION,
            "Unable to update test str. Amount needed for testStr exceeds its size"
        );
        return;
    }
    size_t i = 0;
    for (size_t y = 0; y < board->height; y++) {
        for (size_t x = 0; x < board->width; x++) {
            C4_SlotState state = C4_Board_GetSlot(board, (uint8_t)x, (uint8_t)y);
            strToUpdate[i] = C4_Board_GetCharForState(state);
            i++;
        }
        strToUpdate[i] = '\n';
        i++;
    }
    // \0 marks where the string stops.
    strToUpdate[i] = '\0';
}

bool C4_Board_IsEmpty(C4_Board* board) {
    assert(board);

    for (size_t i = 0; i < (size_t)board->width * (size_t)board->height; i++) {
        if (board->cells[i] != C4_SlotState_Empty) {
            return false;
        }
    }
    return true;
}

bool C4_Board_IsFull(C4_Board* board) {
    assert(board);
    
    for (size_t i = 0; i < (size_t)board->width * (size_t)board->height; i++) {
        if (board->cells[i] == C4_SlotState_Empty) {
            return false;
        }
    }
    return true;
}

uint8_t C4_Board_GetMaxAmountToWin(uint8_t width, uint8_t height) {
    return (uint8_t)C4_Max(width, height);
}