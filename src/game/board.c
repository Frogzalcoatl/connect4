#include <stdlib.h>
#include "Connect4/game/board.h"

Board Board_create(uint8_t width, uint8_t height) {
    Board newBoard;
    newBoard.width = width;
    newBoard.height = height;
    // Put cells on the heap to support any width/height
    // If cells were on the stack, I would have to set a max array size
    // Would be wasteful if i set the max to 100x100 for example but only used 6x7.
    newBoard.cells = calloc(width * height, sizeof(SlotState));
    return newBoard;
}

void Board_destroy(Board* board) {
    if (board->cells != NULL) {
        // must free the cells directly since theyre on the heap
        free(board->cells);
        board->cells = NULL;
        // When main returns, instances of board and their stack
        // allocated properties are destroyed automatically.
    }
}

SlotState Board_get_slot(Board* board, uint8_t x, uint8_t y) {
    if (x >= board->width || y >= board->height) {
        return Empty;
    }
    return board->cells[board->width * y + x];
}

bool Board_set_slot(Board* board, uint8_t x, uint8_t y, SlotState state) {
    if (x >= board->width || y >= board->height) {
        return false;
    }
    board->cells[board->width * y + x] = state;
    return true;
}

bool Board_do_move(Board* board, uint8_t inColumn, SlotState player) {
    if (player == Empty || inColumn >= board->width) {
        return false;
    }
    // If column is full returns false
    if (board->cells[inColumn] != Empty) {
        return false;
    }
    for (size_t row = board->height; row != SIZE_MAX; row--) {
        if (board->cells[row * board->width + inColumn] == Empty) {
            board->cells[row * board->width + inColumn] = player;
            return true;
        }
    }
    return false;
}

SlotState Board_get_winner(Board* board) {
    // Ill implement this later wanna try the test string thing
    return Empty;
}

char get_char_for_state(SlotState state) {
    switch (state) {
        case Player1: return 'X';
        case Player2: return 'O';
        case Empty: return '.';
        default: return '?';
    }
}

void Board_update_test_string(Board* board, char* buffer) {
    size_t i = 0;
    for (size_t y = 0; y < board->height; y++) {
        for (size_t x = 0; x < board->width; x++) {
            SlotState state = Board_get_slot(board, x, y);
            buffer[i] = get_char_for_state(state);
            i++;
        }
        buffer[i] = '\n';
        i++;
    }
    // \0 marks where the string stops.
    buffer[i] = '\0';
}