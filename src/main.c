#include "Connect4/connect4.h"
#include <SDL3/SDL_main.h>

int main(int argc, char *argv[]) {
    // To shutup compiler warnings
    (void)argc;
    (void)argv;
    if (!Connect4_Init_Dependencies()) {
        SDL_Log("Unable to initialize dependencies");
        return 1;
    }
    C4_Game* connect4 = C4_Game_Create(7, 6, 4);
    if (connect4) {
        // I like logs starting under the file path of the exe in my ide.
        SDL_Log("");
        C4_Game_Run(connect4);
        C4_Game_Destroy(connect4);
    }
    Connect4_Quit_Dependencies();
    return 0;
}