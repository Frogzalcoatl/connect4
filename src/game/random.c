#include "Connect4/game/random.h"
#include <stdlib.h>

int C4_GetRandomInt(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}