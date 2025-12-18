#include <SDL3/SDL.h>
#include "Connect4/game/board.h"
#include "Connect4/game/utils.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

C4_Board* C4_Board_Create(uint8_t width, uint8_t height, uint8_t amountToWin) {
    if (amountToWin <= 1) {
        SDL_Log("amount to win cannot be less than 1");
        return NULL;
    }
    C4_Board* newBoard = calloc(1, sizeof(C4_Board));
    // Put cells on the heap to support any width/height
    // If cells were on the stack, I would have to set a max array size
    // Would be wasteful if i set the max to 100x100 for example but only used 6x7.
    newBoard->cells = calloc(width * height, sizeof(C4_SlotState));
    if (!newBoard->cells) {
        C4_Board_Destroy(newBoard);
        SDL_Log("Unable to allocate memory for board cells");
        return NULL;
    }
    newBoard->cellCheckBufferSize = (size_t)C4_Max((int)width, (int)height);
    newBoard->cellCheckBuffer = calloc(newBoard->cellCheckBufferSize, sizeof(C4_SlotState));
    newBoard->cellCheckCount = 0;
    newBoard->width = width;
    newBoard->height = height;
    newBoard->amountToWin = amountToWin;
    // Randomly select either player 1 or 2
    newBoard->currentPlayer = C4_GetRandomInt(1, 2);
    return newBoard;
}

void C4_Board_Destroy(C4_Board* board) {
    if (!board) {
        SDL_Log("Tried to destroy NULL board");
        return;
    }
    if (board->cells) {
        // must free the cells directly since theyre on the heap
        free(board->cells);
    }
    if (board->cellCheckBuffer) {
        free(board->cellCheckBuffer);
    }
    free(board);
}

void C4_Board_Reset(C4_Board* board) {
    if (!board) {
        return;
    }
    for (size_t i = 0; i < board->width * board->height; i++) {
        board->cells[i] = C4_SlotState_Empty;
    }
}

C4_SlotState C4_Board_GetSlot(C4_Board* board, uint8_t x, uint8_t y) {
    if (!board) {
        SDL_Log("Board is NULL");
        return C4_SlotState_Empty;
    }
    if (x >= board->width || y >= board->height) {
        SDL_Log("Tried to access slot outside of bounds");
        return C4_SlotState_Empty;
    }
    return board->cells[board->width * y + x];
}

bool C4_Board_SetSlot(C4_Board* board, uint8_t x, uint8_t y, C4_SlotState state) {
    if (!board) {
        SDL_Log("Board is NULL");
        return false;
    }
    if (x >= board->width || y >= board->height) {
        SDL_Log("Tried to access slot outside of bounds");
        return false;
    }
    board->cells[board->width * y + x] = state;
    return true;
}

static void C4_Board_SwapCurrentPlayer(C4_Board* board) {
    if (!board) {
        SDL_Log("Board is NULL");
        return;
    }
    if (board->currentPlayer == C4_SlotState_Player1) {
        board->currentPlayer = C4_SlotState_Player2;
    } else {
        board->currentPlayer = C4_SlotState_Player1;
    }
}

// Returns index of move. -1 if move is invalid.
int64_t C4_Board_DoMove(C4_Board* board, uint8_t inColumn) {
    if (!board) {
        SDL_Log("Board is NULL");
        return -1;
    }
    if (board->currentPlayer == C4_SlotState_Empty || inColumn >= board->width) {
        SDL_Log("Unable to do board move");
        return -1;
    }
    // If column is full returns -1
    if (board->cells[inColumn] != C4_SlotState_Empty) {
        return -1;
    }
    for (size_t row = board->height - 1; row < board->height; row--) {
        if (board->cells[row * board->width + inColumn] == C4_SlotState_Empty) {
            board->cells[row * board->width + inColumn] = board->currentPlayer;
            C4_Board_SwapCurrentPlayer(board);
            return (long long)row * board->width + (long long)inColumn;
        }
    }
    return -1;
}

static char C4_Board_GetCharForState(C4_SlotState state) {
    switch (state) {
        case C4_SlotState_Player1: return 'X';
        case C4_SlotState_Player2: return 'O';
        case C4_SlotState_Empty: return '.';
        default: return '?';
    }
}

static void C4_Board_PrintCellCheckBuffer(C4_Board* board) {
    if (!board) {
        SDL_Log("Board is NULL");
        return;
    }
    const size_t strSize = board->cellCheckCount * 2 + 1;
    char* str = malloc(strSize);
    if (!str) {
        SDL_Log("Unable to allocate memory for cellCheckBuffer string");
        return;
    }
    for (size_t i = 0; i < board->cellCheckCount; i++) {
        str[i * 2] = C4_Board_GetCharForState(board->cellCheckBuffer[i]);
        str[i * 2 + 1] = ' ';
        
    }
    str[strSize - 1] = '\0';
    SDL_Log("%s", str);
    free(str);
}

static void C4_Board_UpdateCellCheckBuffer(C4_Board* board, C4_Board_RowAxis axis, size_t atIndex) {
    if (!board) {
        SDL_Log("Board is NULL");
        return;
    }
    // Sets all values of cellCheckBuffer to zero.
    memset(board->cellCheckBuffer, 0, (size_t)C4_Max((int)board->width, (int)board->height) * sizeof(C4_SlotState));
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
                if (nextIndex == (board->width * board->height) - 1) {
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
            }
        }; break;
    }
}

static C4_SlotState C4_Board_TestCellCheckBuffer(C4_Board* board) {
    if (!board) {
        SDL_Log("Board is NULL");
        return C4_SlotState_Empty;
    }
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
    if (!board) {
        SDL_Log("Board is NULL");
        return C4_SlotState_Empty;
    }
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

void C4_Board_UpdateTestStr(C4_Board* board, char* buffer, size_t bufferSize) {
    if (!board) {
        SDL_Log("Board is NULL");
        return;
    }
    size_t amountNeeded = board->width * board->height + board->height + 1;
    if (amountNeeded > bufferSize) {
        SDL_Log("Amount needed for testStr exceeds its size");
        return;
    }
    size_t i = 0;
    for (size_t y = 0; y < board->height; y++) {
        for (size_t x = 0; x < board->width; x++) {
            C4_SlotState state = C4_Board_GetSlot(board, (uint8_t)x, (uint8_t)y);
            buffer[i] = C4_Board_GetCharForState(state);
            i++;
        }
        buffer[i] = '\n';
        i++;
    }
    // \0 marks where the string stops.
    buffer[i] = '\0';
}