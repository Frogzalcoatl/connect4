#include "Connect4/game/utils.h"
#include <stdlib.h>

int C4_GetRandomInt(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

int C4_Max(int a, int b) {
    return a > b ? a : b;
}

int C4_Min(int a, int b) {
    return a < b ? a : b;
}