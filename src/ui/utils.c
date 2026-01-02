#include "Connect4/ui/utils.h"
#include <stdlib.h>

int C4_Max(int a, int b) {
    return a > b ? a : b;
}

int C4_Min(int a, int b) {
    return a < b ? a : b;
}