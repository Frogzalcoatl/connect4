#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Connect4/connect4.h"

int main(int argc, char *argv[]) {
    if (!Connect4_Init_Dependencies()) {
        return 1;
    }
    Connect4* connect4 = Connect4_create(7, 6);
    if (connect4) {
        Connect4_run(connect4);
        Connect4_destroy(connect4);
    }
    Connect4_Quit_Dependencies();
    return 0;
}