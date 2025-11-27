#include <stdlib.h>
#include "Connect4/game/board.h"
#include "Connect4/game/random.h"
#include <stddef.h>

C4_Board C4_Board_Create(uint8_t width, uint8_t height) {
    C4_Board newBoard;
    newBoard.width = width;
    newBoard.height = height;
    // Put cells on the heap to support any width/height
    // If cells were on the stack, I would have to set a max array size
    // Would be wasteful if i set the max to 100x100 for example but only used 6x7.
    newBoard.cells = calloc(width * height, sizeof(C4_SlotState));
    // Randomly select either player 1 or 2
    newBoard.currentPlayer = C4_GetRandomInt(1, 2);
    return newBoard;
}

void C4_Board_Destroy(C4_Board* board) {
    if (board->cells != NULL) {
        // must free the cells directly since theyre on the heap
        free(board->cells);
        board->cells = NULL;
        // When main returns, instances of board and their stack
        // allocated properties are destroyed automatically.
    }
}

C4_SlotState C4_Board_GetSlot(C4_Board* board, uint8_t x, uint8_t y) {
    if (x >= board->width || y >= board->height) {
        return C4_SlotState_Empty;
    }
    return board->cells[board->width * y + x];
}

bool C4_Board_SetSlot(C4_Board* board, uint8_t x, uint8_t y, C4_SlotState state) {
    if (x >= board->width || y >= board->height) {
        return false;
    }
    board->cells[board->width * y + x] = state;
    return true;
}

static void C4_Board_SwapCurrentPlayer(C4_Board* board) {
    if (board->currentPlayer == C4_SlotState_Player1) {
        board->currentPlayer = C4_SlotState_Player2;
    } else {
        board->currentPlayer = C4_SlotState_Player1;
    }
}

bool C4_Board_DoMove(C4_Board* board, uint8_t inColumn) {
    if (board->currentPlayer == C4_SlotState_Empty || inColumn >= board->width) {
        return false;
    }
    // If column is full returns false
    if (board->cells[inColumn] != C4_SlotState_Empty) {
        return false;
    }
    for (size_t row = board->height - 1; row < board->height; row--) {
        if (board->cells[row * board->width + inColumn] == C4_SlotState_Empty) {
            board->cells[row * board->width + inColumn] = board->currentPlayer;
            C4_Board_SwapCurrentPlayer(board);
            return true;
        }
    }
    return false;
}

C4_SlotState C4_Board_GetWinner(C4_Board* board) {
    // Ill implement this later wanna try the test string thing
    // I only need to check the lines of the most recently placed piece rather than the whole board
    return C4_SlotState_Empty;
}

// Static prevents this function from being global
static char get_char_for_state(C4_SlotState state) {
    switch (state) {
        case C4_SlotState_Player1: return 'X';
        case C4_SlotState_Player2: return 'O';
        case C4_SlotState_Empty: return '.';
        default: return '?';
    }
}

void C4_Board_UpdateTestStr(C4_Board* board, char* buffer, size_t bufferSize) {
    size_t amountNeeded = board->width * board->height + board->height + 1;
    if (amountNeeded > bufferSize) {
        return;
    }
    size_t i = 0;
    for (size_t y = 0; y < board->height; y++) {
        for (size_t x = 0; x < board->width; x++) {
            C4_SlotState state = C4_Board_GetSlot(board, x, y);
            buffer[i] = get_char_for_state(state);
            i++;
        }
        buffer[i] = '\n';
        i++;
    }
    // \0 marks where the string stops.
    buffer[i] = '\0';
}