#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    Empty,
    Player1,
    Player2
} SlotState;

typedef struct {
    uint8_t width;
    uint8_t height;
    SlotState* cells;
} Board;

Board Board_create(uint8_t width, uint8_t height);
void Board_destroy(Board* board);
SlotState Board_get_slot(Board* board, uint8_t x, uint8_t y);
bool Board_set_slot(Board* board, uint8_t x, uint8_t y, SlotState state);
// Returns true if valid move was made
bool Board_do_move(Board* board, uint8_t inColumn, SlotState player);
void Board_update_test_string(Board* board, char* testStr);
SlotState Board_get_winner(Board* board);